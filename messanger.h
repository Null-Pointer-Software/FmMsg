/**
 * Copyright (c) 2014 Null Pointer Software
 * All rights reserved.
 *
 * Developed by: 		Null Pointer Software Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal with the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * 1 Redistributions of source code must retain the above copyright notice, this list of conditions and
 *   the following disclaimers.
 * 2 Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *   and the following disclaimers in the documentation and/or other materials provided with the distribution.
 * 3 Neither the names of <Name of Development Group, Name of Institution>, nor the names of its contributors
 *   may be used to endorse or promote products derived from this Software without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
 */

/**
 * This file is a collection of messaging platform service function calls. All functions are
 * defined as inline.
 */

#ifndef Messanger_H
#define Messagner_H

#include "FmMsg.h"

#include <map>

static std::map<char *, FmMsg *> messanger_tally;

inline bool Fm_Register(char * global_name, FmMsg * instance) {
	std::map<char *, FmMsg *>::iterator itr = messanger_tally.find(global_name);
	if (itr != messanger_tally.end()) {
		// global name found
		if (itr->second == instance) // double register
			return true;
		else return false; // global name already occupied
	} else {
		messanger_tally[global_name] = instance;
		return true;
	}
}

inline bool Fm_UnRegister(char * global_name) {
	std::map<char *, FmMsg *>::iterator itr = messanger_tally.find(global_name);
	if (itr == messanger_tally.end()) {
		// it is not found
		return true;
	} else {
		messanger_tally.erase(itr);
		return true;
	}
}

inline bool Fm_Send(char * src, char * dst, void * data, int size, FmMsgEntry_type type = RAW) {
	bool ret = false;
	std::map<char *, FmMsg *>::iterator itr = messanger_tally.find(dst);
	if (itr == messanger_tally.end()) // such entity not exist in map yet
		return ret;

	FmMsg * dstQ = messanger_tally.find(dst)->second;
	FmMsgEntry * data_entry = new FmMsgEntry(size, type);
	data_entry->populate(data, size, type);
	FmMsgQ_entry * entry = new FmMsgQ_entry();
	entry->src = src;
	entry->dst = dst;
	entry->data = data_entry;

	if (dstQ->insert(entry)) {
		ret = true;
	} else ret = false;

	delete data_entry;
	delete entry;

	return ret;
}

#endif // Messanger_H
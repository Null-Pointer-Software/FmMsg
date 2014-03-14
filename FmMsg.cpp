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

#include "FmMsg.h"
#include "messanger.h"

#include <cstddef>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>

FmMsg::FmMsg(char * name, int max, void * (*handle) (FmMsgEntry *)) : _max(max) {
	init(name, handle);
	Fm_Register(name, this);
}

FmMsg::~FmMsg() {
	free(_name);
}

void FmMsg::init(char * name, void * (*handle) (FmMsgEntry *)) {
	_name = (char *) malloc(strlen(name) + 1);
	strcpy(_name, name);
	_name[strlen(name)] = '\0';

	_handle = handle;
}

FmMsgEntry FmMsg::retrieve() {
	if (!lock_data() || _buf.empty()) {
		FmMsgEntry ret(0, RAW);
		return ret;
	} else {
		FmMsgEntry ret(_buf.front(), OWNERSHIP); 
		_buf.pop();
		unlock_data();
		--_cnt;
		return ret;
	}
}

bool FmMsg::retrieve(FmMsgEntry * dst) {
	if (!lock_data() || _buf.empty()) {
		return false;
	} else {
		dst = &_buf.front();
		unlock_data();
		return true;
	}
}

bool FmMsg::retrieve_next(FmMsgEntry * dst) {
	FmMsgEntry src = retrieve();
	dst = &src;
	return (src.getProperty().size == 0) ? false : true;
}

bool FmMsg::insert(FmMsgEntry * src) {
	if (_cnt == _max) {
		_handle(src);
		return false;
	}

	while(!lock_data());
	_buf.push(*src);
	unlock_data();

	++_cnt;

	return true;
}

bool FmMsg::insert(int size, void * content, FmMsgEntry_type type) {
	FmMsgEntry dst(size, type);
	dst.populate(content, size, type);

	if (_cnt == _max) {
		_handle(&dst);
		return false;
	}

	while (!lock_data()); // wait for the lock.
	_buf.push(dst);
	unlock_data();

	++_cnt;

	return true;
}

bool FmMsg::insert(FmMsgQ_entry * src) {
	if (!strcmp(_name, src->dst)) {
		FmMsgEntry dst(*(src->data), OWNERSHIP);

		if (_cnt == _max) {
			_handle(src->data);
			return false;
		}

		while (!lock_data()); // wait for the lock.
		_buf.push(dst);
		unlock_data();

		++_cnt;

		_flag = true;
	} else {
		_flag = false;
	}

	return _flag;
}
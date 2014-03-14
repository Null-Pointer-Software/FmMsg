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

#include "FmMsgEntry.h"

FmMsgEntry::FmMsgEntry(int size, FmMsgEntry_type dtype = RAW) {
	init(size, dtype);
}

FmMsgEntry::FmMsgEntry(FmMsgEntry &src, transfer_type tr_type) {
	int_prop_t prop = src.getProperty();
	_size = prop.size;
	_type = prop.tr_type;

	if (_buff != NULL)
		free(_buff);

	if (tr_type == VALUE)
		_buff = (unsigned char *) malloc(_size * sizeof(unsigned char));

	src.transfer(_buff, tr_type);

	delete &src;
};

FmMsgEntry::~FmMsgEntry() {
	if (_buff != NULL) {
		free(_buff);
	}
};

void FmMsgEntry::init(int size, FmMsgEntry_type dtype) {
	if (_buff != NULL) {
		free(_buff);
	}
	_buff = NULL;
	_size = size;
	_type = dtype;
	_buff = (unsigned char *) malloc(_size * sizeof(unsigned char));
};

bool FmMsgEntry::populate(void * data, int size, FmMsgEntry_type ptype) {
	if (ptype != _type)	// wrong type. reject
		return false;
	else if (size > _size) // not enough space, auto expand
		resize(size);

	memcpy(_buff, data, size);
	return true;
}

FmMsgEntry_type FmMsgEntry::retrieve(void * buf) {
	if (_buff == NULL)
		return NUM_ENTRY_TYPE;
	switch(_type) {
	case INT:
	case STRING:
	case RAW:
	default:
		memcpy(_buff, buf, _size);
	}

	return _type;
}

bool FmMsgEntry::resize(int new_size) {
	if (_buff != NULL) {
		free(_buff);
	}

	init(new_size, _type);

	return true;
}

bool FmMsgEntry::retype(FmMsgEntry_type new_dtype) {
	_type = new_dtype;
	return true;
}

int_prop_t FmMsgEntry::getProperty() {
	int_prop_t ret;
	ret.size = _size;
	ret.tr_type = _type;
	return ret;
}

bool FmMsgEntry::transfer(unsigned char * dst, transfer_type tr_type) {
	if (_buff == NULL)
		return false;
	else if (dst == NULL & tr_type == VALUE)
		return false;
	else {
		switch(tr_type) {
		case OWNERSHIP:
			dst = _buff;
			_buff = NULL;
			break;
		case VALUE:
			memcpy(dst, _buff, _size);
			break;
		}
		return true;
	}
}

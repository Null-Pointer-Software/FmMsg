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

#ifndef FmMsgEntry_H
#define FmMsgEntry_H

#include <cstdlib>
#include <cstdio>
#include <iostream>

/**
 * message entry class
 */
typedef enum FmMsgEntry_type_e{
	INT,				/** < integer type */
	STRING,				/** < string type */
	RAW,				/** < raw data type */

	NUM_ENTRY_TYPE
} FmMsgEntry_type;

typedef struct int_prop_s {
	int size;
	FmMsgEntry_type tr_type;
} int_prop_t;

typedef enum transfer_type_e {
	OWNERSHIP,			/** < transfer over ownership */
	VALUE,				/** < transfer over value only */
} transfer_type;

class FmMsgEntry {
private:
	unsigned char * _buff;	/** < message buffer holding content */
	unsigned int _size;		/** < intended buffer size in bytes. this will determine the buffer size */
	FmMsgEntry_type _type;	/** < incoming message type */

public:
	FmMsgEntry() {};
	FmMsgEntry(int size, FmMsgEntry_type dtype = RAW);
	/**
	 * in copy construction, the buffer content will be transfered and original buffer destroyed
	 */
	FmMsgEntry(FmMsgEntry &src, transfer_type tf_type = OWNERSHIP);

	~FmMsgEntry();

	/**
	 * initialize buffer with given size. if buffer type not assigned, raw buffer assumed.
	 */
	void init(int size, FmMsgEntry_type dtype = RAW);

	/**
	 * populate internal buffer with the given content. if the buffer is occupied, the original
	 * content will be wipped
	 */
	bool populate(void * data, int size, FmMsgEntry_type ptype = RAW);

	/**
	 * retrieve internal buffer content
	 */
	FmMsgEntry_type retrieve(void * buf);

	/**
	 * resize buffer to new_size. does not change buffer type.
	 */
	bool resize(int new_size);

	/**
	 * retype buffer to new buffer type. if type not assigned, reset type to RAW.
	 */
	bool retype(FmMsgEntry_type dtype = RAW);

	/**
	 * return internal property.
	 */
	int_prop_t getProperty();

	/**
	 * transfer buffer, behavior branched based on transfer type
	 * NOTE: if transfer ownership current object buffer will be reset to NULL
	 *       if transfer value, caller is responsible to give dst enough space
	 */
	bool transfer(unsigned char * dst, transfer_type tr_type = OWNERSHIP);
};

#endif // FmMsgEntry_H

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

#ifndef FmMsg_H
#define FmMsg_H

#include <queue>
#include <pthread.h>
#include "FmMsgEntry.h"

typedef struct FmMsgQ_entry {
	FmMsgEntry * data;	/** < the actual content of the queue entry */
	char * src;			/** < the integer assigned to the source. usually the unsigned representation of source ip | port */
	char * dst;			/** < the integer assigned to the destination. usually the unsigned representation of dst ip | port */
} FmMsgQ_entry;

/**
 * message queue class providing necessary interfaces for messanger platform.
 * the assumption is that all communication will be through local socket.
 */
class FmMsg {
private:
	std::queue<FmMsgEntry> _buf;		/** < internal queue structure */
	pthread_mutex_t _msgMutex;			/** < mutex structure to block content read/write */
	bool _flag;							/** < internal flag signaling queue content availability */
	void * (*_handle) (FmMsgEntry *);	/** < function handle for overflown elements */
	char * _name;						/** < string representation of global name for this queue */
	const int _max;						/** < internal counter limit*/
	int _cnt;							/** < internal counter */

	inline bool lock_data() { return (pthread_mutex_trylock(&_msgMutex) == 0) ? true : false; }
	inline void unlock_data() { pthread_mutex_unlock(&_msgMutex); }
public:
	FmMsg(char * name, int max = 1024, void * (*handle) (FmMsgEntry *) = NULL);
	~FmMsg();

	/**
	 * init function initializing the message queue.
	 * the following is a list of required information:
	 * 1. char * name -> public name for socket binding
	 * 2. int max -> an integer count of maximum queued messages allowed, by default it is 1024
	 * 3. void * (*shandle) (void *) -> provided handler when message queue is full
	 *									allow special handling of overflown elements
	 */
	void init(char * name, void * (*handle) (FmMsgEntry *) = NULL);

	/**
	 * retrieve the head of the queue
	 */
	FmMsgEntry retrieve(); /** < this function returns the element and remove it from the head */
	bool retrieve(FmMsgEntry * dst); /** < this function return the pointer to the head of queue but does not remove
										   it. note that caller is responsible for providing enough space for dst*/
	bool retrieve_next(FmMsgEntry * dst); /** < same as bool retrieve(FmMsgEntry * dst) but removes the element */

	/**
	 * insert an element to the end of the queue
	 */
	bool insert(FmMsgEntry * src);
	bool insert(int size, void * content, FmMsgEntry_type entry_type = RAW);
	bool insert(FmMsgQ_entry * src); /** < this function is called by messanger. the data will contain desired
										   content to be saved, as well as information regarding src and dst.
										   if dst is not current queue, a message will be returned. */
};

#endif // FmMsg_H

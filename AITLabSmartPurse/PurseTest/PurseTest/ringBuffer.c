/*
 * ringBuffer.c
 *
 * Created: 6/29/2019 8:49:40 PM
 *  Author: Rawr
 */ 

#include "include.h"


void ringBuffInit(ringBuffer *_this){
	//set buffer elements to zero.
	//also sets head, tail, and count to zero.
	memset(_this, 0, sizeof(*_this));
}

uint8_t ringBuffEmpty(ringBuffer *_this){
	return (0==_this->count);
}

uint8_t ringBuffFull(ringBuffer *_this){
	return(_this->count >= RBUFF_SIZE);
}

//if queue is empty then return -1
uint8_t ringBuffGet(ringBuffer *_this){
	int c;
	if(_this->count > 0){
		c = _this->buff[_this->tail];
		//rollover counter.  Once the counter reaches
		// RBUFF_SIZE, then the counter goes to zero.
		_this->tail = (_this->tail+1)%RBUFF_SIZE;
		--_this->count;
		}else{
		c=-1;
	}
	return (c);
}

void ringBuffPut(ringBuffer *_this, const uint8_t c){
	if(_this->count < RBUFF_SIZE){
		_this->buff[_this->head] = c;
		_this->head = (_this->head+1)%RBUFF_SIZE;
		++_this->count;
	}
	//keep this if you want to overwrite the buffer when the buffer is full
	//comment out else statement to lose data when buffer is full.
	else{
		_this->buff[_this->head] = c;
		_this->head = (_this->head+1)%RBUFF_SIZE;
		_this->tail = (_this->tail+1)%RBUFF_SIZE;
	}
}

void ringBuffFlush(ringBuffer *_this, const uint8_t clearBuffer){
	_this->head = 0;
	_this->tail = 0;
	_this->count = 0;
	if(clearBuffer){
		memset(_this->buff, 0, sizeof(_this->buff));
	}
}

//voltile buffer functions////////////////////////////
void ringBuffInit_v(volatile ringBuffer *_this){
	//set buffer elements to zero.
	//also sets head, tail, and count to zero.
	memset((void*)_this, 0, sizeof(*_this));
}

uint8_t ringBuffEmpty_v(volatile ringBuffer *_this){
	return (0==_this->count);
}

uint8_t ringBuffFull_v(volatile ringBuffer *_this){
	return(_this->count >= RBUFF_SIZE);
}

//if queue is empty then return -1
uint8_t ringBuffGet_v(volatile ringBuffer *_this){
	int c;
	if(_this->count > 0){
		c = _this->buff[_this->tail];
		//rollover counter.  Once the counter reaches
		// RBUFF_SIZE, then the counter goes to zero.
		_this->tail = (_this->tail+1)%RBUFF_SIZE;
		--_this->count;
		}else{
		c=-1;
	}
	return (c);
}

void ringBuffPut_v(volatile ringBuffer *_this, const uint8_t c){
	if(_this->count < RBUFF_SIZE){
		_this->buff[_this->head] = c;
		_this->head = (_this->head+1)%RBUFF_SIZE;
		++_this->count;
	}
	//keep this if you want to overwrite the buffer when the buffer is full
	//comment out else statement to lose data when buffer is full.
	else{
		_this->buff[_this->head] = c;
		_this->head = (_this->head+1)%RBUFF_SIZE;
		_this->tail = (_this->tail+1)%RBUFF_SIZE;
	}
}

void ringBuffFlush_v(volatile ringBuffer *_this, const uint8_t clearBuffer){
	_this->head = 0;
	_this->tail = 0;
	_this->count = 0;
	if(clearBuffer){
		memset((void*)_this->buff, 0, sizeof(_this->buff));
	}
}
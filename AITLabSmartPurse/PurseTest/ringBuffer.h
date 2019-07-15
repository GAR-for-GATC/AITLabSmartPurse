/*
 * ringBuffer.h
 *
 * Created: 6/29/2019 8:49:54 PM
 *  Author: Rawr
 */ 


#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_
	#define RBUFF_SIZE 32
	
	typedef struct ringBuffer{
		uint8_t buff[RBUFF_SIZE]; //the buffer
		uint8_t head;//bytes written are using this
		uint8_t tail;//used to retrieve the oldest data
		uint8_t count;//used to keep track of the total number of items in the index
	}ringBuffer;
	
	//volatile ring buffer
	typedef struct ringBuffer_v{
		uint8_t buff[RBUFF_SIZE]; //the buffer
		uint8_t head;//bytes written are using this
		uint8_t tail;//used to retrieve the oldest data
		uint8_t count;//used to keep track of the total number of items in the index
	}volatile ringBuffer_v;
	
	
	void ringBuffInit(ringBuffer *_this);
	uint8_t ringBuffEmpty(ringBuffer *_this);
	uint8_t ringBuffFull(ringBuffer *_this);
	uint8_t ringBuffGet(ringBuffer *_this);//gets a byte from the queue, from the tail
	void ringBuffPut(ringBuffer *_this, const uint8_t c);//puts a byte in the queue, using the head.
	void ringBufferFlush(ringBuffer *_this, const uint8_t clearBuffer);//turn all elements in the buffer to zero.

	//functions for volatile buffer
	void ringBuffInit_v(volatile ringBuffer *_this);
	uint8_t ringBuffEmpty_v(volatile ringBuffer *_this);
	uint8_t ringBuffFull_v(volatile ringBuffer *_this);
	uint8_t ringBuffGet_v(volatile ringBuffer *_this);//gets a byte from the queue, from the tail
	void ringBuffPut_v(volatile ringBuffer *_this, const uint8_t c);//puts a byte in the queue, using the head.
	void ringBufferFlush_v(volatile ringBuffer *_this, const uint8_t clearBuffer);//turn all elements in the buffer to zero.



#endif /* RINGBUFFER_H_ */
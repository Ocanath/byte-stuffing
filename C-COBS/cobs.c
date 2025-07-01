#include "cobs.h"
/*Note: this implementation assumes that the maximum payload size is always under 253, 
and that manual packetization will be implemented for buffers above this size. */


int cobs_encode(cobs_buf_t * msg)
{
	//Encoding an encoded buffer should be allowed. This could be useful for forwarding to 'stupid' devices with constrained COBS handling (take in stream and forward unstuffed payload)
	if(msg == NULL)
	{
		return -1;	//null pointer check
	}
	//overrun check. we offset the buffer by one to prepend the pointer, and index into length+2 to append the delimiter, so this is critical for memory safety
	if(msg->length + 2 > msg->size || msg->size < COBS_MIN_BUF_SIZE)
	{
		return -1;
	}

	//shift the entire buffer up by one index. 
	//Note: this is unnecessary if cobs_buf_t tracks two pointers; one for 'encoded' and one for 'decoded' on the same run of memory, off by 1. 
	for(int i = msg->length-1; i > 0; i--)
	{
		msg->buf[i+1] = msg->buf[i];
	}
	msg->length += 1;	//increase the length by one before loading the pointers 
	msg->buf[msg->length++] = 0;	//append delimiter byte

	int pointer_idx = 0;
	for(int i = 1; i < msg->length; i++)
	{
		if(msg->buf[i] == 0)
		{
			msg->buf[pointer_idx] = i - pointer_idx;	//we already shift by 1 when we start analyzing the shifted payload, so this load works
			pointer_idx = i;	//the zero bytes are always replaced by pointers
		}
	}

	msg->state = COBS_ENCODED;
	return 0;
}


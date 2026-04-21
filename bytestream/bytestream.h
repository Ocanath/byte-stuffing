#ifndef BYTESTREAM_H
#define BYTESTREAM_H
#include <stddef.h>
#include <stdint.h>

typedef struct bytestream_buf_t
{
    unsigned char * buf;	//pointer to the first address of the byte array backing store
    size_t size;			//size of the backing store buf points to.
    size_t len;				//length of the message within buf. Reported value. Only valid after returning STREAM_SUCCESS - stale otherwise
	size_t pos;				//position, for stream buffering purposes. 
}bytestream_buf_t;

enum {
	STREAM_SUCCESS = 0,
	STREAM_IN_PROGRESS = 1,
	STREAM_ERROR_NULLPTR = -1,
	STREAM_ERROR_OVERRUN = -2,
	STREAM_ERROR_EMPTYBUFFER = -3
};

int bytestream(unsigned char new_byte, bytestream_buf_t * input, unsigned char delimiter);					//for cobs decode
int bytestream_dual_delimiter(unsigned char new_byte, bytestream_buf_t * input, unsigned char delimiter);	//for ppp unstuff

#endif

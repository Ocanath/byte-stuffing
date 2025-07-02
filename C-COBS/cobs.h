#ifndef COBS_STUFFING_H
#define COBS_STUFFING_H
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define COBS_MIN_BUF_SIZE	3

enum
{
	COBS_STREAMING_COMPLETE,	//delimiter found, parsing required. 
	 COBS_STREAMING_PARSED,
	  COBS_STREAMING_IN_PROGRESS
};  //streaming state

enum {COBS_DECODED, COBS_ENCODED};  //encoding state
enum {COBS_SUCCESS = 0, COBS_ERROR_NULL_POINTER = -1, COBS_ERROR_SIZE = -2, COBS_ERROR_POINTER_OVERFLOW = -4, COBS_ERROR_SERIAL_OVERRUN = -5, COBS_ERROR_STREAMING_FRAME_DROPPED = -6};


typedef struct cobs_buf_t
{
    unsigned char * buf;
    size_t size;
    int length;
    uint8_t encoded_state;
    uint8_t streaming_state;
} cobs_buf_t;

//Function template for basic cobs decoding. Takes an ENCODED buffer and decodes it into a DECODED buffer.
int cobs_decode(cobs_buf_t * msg);
//Function template for basic cobs encoding. Takes a DECODED buffer and encodes it into an ENCODED buffer.
int cobs_encode(cobs_buf_t * msg);
//Function template to handle an incoming byte stream. Uses ZERO as the delimiter, and decodes the stream into a DECODED buffer.
int cobs_stream(unsigned char new_byte, cobs_buf_t * msg);

#endif
#ifndef COBS_STUFFING_H
#define COBS_STUFFING_H
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define COBS_MIN_BUF_SIZE	3


typedef enum {COBS_DECODED, COBS_ENCODED} cobs_state_t;

typedef struct cobs_buf_t
{
    unsigned char * buf;
    size_t size;
    int length;
    cobs_state_t state;
} cobs_buf_t;

//Function template for basic cobs decoding. Takes an ENCODED buffer and decodes it into a DECODED buffer.
int cobs_decode(cobs_buf_t *cobs_buf);
//Function template for basic cobs encoding. Takes a DECODED buffer and encodes it into an ENCODED buffer.
int cobs_encode(cobs_buf_t *cobs_buf);
//Function template to handle an incoming byte stream. Uses ZERO as the delimiter, and decodes the stream into a DECODED buffer.
int cobs_stream(unsigned char new_byte, cobs_buf_t *cobs_buf);

#endif
#ifndef PPP_H
#define PPP_H

#include <stdint.h>

#define FRAME_CHAR 0x7E
#define ESC_CHAR 0x7D
#define ESC_MASK 0x20

typedef struct buffer_t
{
    unsigned char * buf;
    size_t size;
    int length;
} buffer_t;

int PPP_stuff(buffer_t * unstuffed_buffer, buffer_t * stuffed_buffer);
int PPP_unstuff( buffer_t * unstuffed_buffer, buffer_t * stuffed_buffer);
int parse_PPP_stream(uint8_t new_byte, buffer_t * unstuffed_buffer, buffer_t * input_buffer);

#endif

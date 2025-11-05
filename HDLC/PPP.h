#ifndef PPP_H
#define PPP_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FRAME_CHAR 0x7E
#define ESC_CHAR 0x7D
#define ESC_MASK 0x20

typedef struct ppp_buffer_t
{
    unsigned char * buf;
    size_t size;
    size_t length;
} ppp_buffer_t;

int PPP_stuff(ppp_buffer_t * unstuffed_buffer, ppp_buffer_t * stuffed_buffer);
int PPP_unstuff( ppp_buffer_t * unstuffed_buffer, ppp_buffer_t * stuffed_buffer);
int parse_PPP_stream(uint8_t new_byte, ppp_buffer_t * unstuffed_buffer, ppp_buffer_t * input_buffer);

#ifdef __cplusplus
}
#endif

#endif

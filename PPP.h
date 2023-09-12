#include <stdint.h>

#define FRAME_CHAR 0x7D
#define ESC_CHAR 0x7E
#define ESC_MASK 0x20

#define BUFFER_SIZE 128


extern uint8_t gl_stuffing_buffer[BUFFER_SIZE];

int PPP_stuff(uint8_t * payload, int payload_size, uint8_t * stuffed_buffer, int stuffed_buffer_size);
int PPP_unstuff( uint8_t * payload, int payload_buffer_size, uint8_t * stuffed_buffer, int stuffed_buffer_length);



#include <stdint.h>

typedef struct dma_registers_t
{
    uint32_t CNDTR; //count-down register
    unsigned char * CMAR;   //
    uint32_t size; //
}dma_registers_t;


void enqueue_byte(unsigned char byte, dma_registers_t * dma);

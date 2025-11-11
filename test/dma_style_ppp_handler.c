#include "PPP.h"
#include "dma_style_ppp_handler.h"

/*Virtualize hardware behavior*/
void enqueue_byte(unsigned char byte, dma_registers_t * dma)
{
    if(dma->CNDTR > 0)
    {
        unsigned char * pbyte = dma->CMAR + (dma->size - dma->CNDTR);
        *pbyte = byte;
        dma->CNDTR--;
    }
}



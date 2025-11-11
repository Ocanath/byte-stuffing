#include "PPP.h"
#include "dma_style_ppp_handler.h"

/*Virtualize hardware behavior*/
void enqueue_byte(unsigned char byte, dma_registers_t * dma)
{
    if(dma->CNDTR == 0)
    {
        dma->p_current = dma->CMAR;
        return;
    }
    dma->CNDTR--;
    *dma->p_current = byte;
    dma->p_current++;
}



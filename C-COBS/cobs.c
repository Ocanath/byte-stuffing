#include "cobs.h"

int cobs_encode(cobs_buf_t *cobs_buf)
{
	//Encoding an encoded buffer should be allowed. This could be useful for forwarding to 'stupid' devices with constrained COBS handling (take in stream and forward unstuffed payload)
	if(cobs_buf == NULL)
	{
		return -1;	//null pointer check
	}


}
#include "PPP.h"

/*
	Takes a payload and stuffs it based on PPP byte stuffing protocol. 
	Result is available in a global fixed stuffing buffer
	
	Returns the size of the stuffed buffer on success
	If there is a memory overrun, returns 0 to indicate failure
	
	Inputs:
		payload: buffer containing the data to be byte stuffed
		payload_size: the length of the data payload
		stuffed_buffer_size: the length of the working buffer. For bounds check/preventing memory overrun of the working buffer
	Outputs:
		stuffed_buffer: the working buffer containing the stuffed result
		returns: the length of the stuffed buffer, or 0 if the stuffed buffer is overrun
		
*/
int PPP_stuff(ppp_buffer_t * unstuffed_buffer, ppp_buffer_t * stuffed_buffer)
{
	int bidx = 0;
	stuffed_buffer->buf[bidx++] = FRAME_CHAR;
	for(int i = 0; i < unstuffed_buffer->length; i++)
	{
		uint8_t b = unstuffed_buffer->buf[i];
		if( (b == FRAME_CHAR) || (b == ESC_CHAR) )
		{
			if(bidx + 1 >= stuffed_buffer->size)
				return 0;
			stuffed_buffer->buf[bidx++] = ESC_CHAR;
			stuffed_buffer->buf[bidx++] = b ^ ESC_MASK;
		}
		else
		{
			if(bidx >= stuffed_buffer->size)
				return 0;
			stuffed_buffer->buf[bidx++] = b;
		}
	}
	stuffed_buffer->buf[bidx++] = FRAME_CHAR;
	stuffed_buffer->length = bidx;
	return bidx;
}


/*
	Note: This will cause a memory overrun if the size of the payload is less than half the stuffed buffer size + 2
	This is the reciprocal function of stuff_pkt
	
	Inputs: 
		stuffed_buffer: the payload, which has been stuffed
		stuffed_buffer_length: the length of the stuffed data. 
			Must be less than the actual bounds of the stuffed buffer, 
			but the function may return without scanning up to this range.
			Function will return after the first valid payload frame detected in the stuffed buffer
		payload_size: the upper bound on the size of the working buffer, payload. prevents memory overrun. Can be larger than the actual payload size, which is a returned value
		
	Outputs:
		payload: the working buffer. contains resulting unstuffed data after function returns successfully
		returns: the actual size of the payload, after unstuffing operation is complete. returns 0 on failure




	!!!!!!!!!!!!!!!!!!!!!IMPORTANT!!!!!!!!!!!!!!!!!!!!!
	DON'T TRUST THIS FUNCTION. When I switched to buffer_t, 
	I used ->length instead of *bidx for stream index tracking and 
	i'm not sure that works correctly.

	This function requires explicit unit testing. Once I add it i'll delte this comment.
	TODO: what you have to do to clear this comment.
*/
int PPP_unstuff( ppp_buffer_t * unstuffed_buffer, ppp_buffer_t * stuffed_buffer)
{
	if(stuffed_buffer->buf[0] != FRAME_CHAR)
		return 0;
	int pld_idx = 0;	//payload/working buffer index, starts at 0
	for(int i = 1; i < stuffed_buffer->length; i++)
	{
		 if(stuffed_buffer->buf[i] == ESC_CHAR)	//marks prepend of xored data. xor again to recover the original value
		 {
			 i++; //skip to the next value
			 if(i >= stuffed_buffer->length || pld_idx >= unstuffed_buffer->size)	//memory overrun guards. do two because we could overrun the while loop guards here
				 return 0;
			 unstuffed_buffer->buf[pld_idx++] = stuffed_buffer->buf[i] ^ ESC_MASK;
		 }
		 else if(stuffed_buffer->buf[i] == FRAME_CHAR)	//end of buffer, return 
		 {
			 return pld_idx;	
		 }
		 else	//unaffected data, 'normal' case
		 {
			 if(pld_idx >= unstuffed_buffer->size)
				 return 0;
			 unstuffed_buffer->buf[pld_idx++] = stuffed_buffer->buf[i];
		 }
	}
	return 0; //we have overrun the stuffed buffer without finding a frame character, meaning the buffer is improperly formed. return 0 length because payload is also invalid
}

/*
* For an incoming stream of data, i.e. from a serial port
* Returns payload size when the payload buffer contains a valid copy of the payload, and 0 otherwise
* 
* On an embedded system, this would be in a main loop after data is offloaded from an interrupt handler (arduino), or DMA (generic DMA on STM32 not using idle line detection for framing),
* or in an interrupt handler (generic microcontroller implementation). Creating an arduino-like queue handling between this process in main and the interrupt handler
* is preferred to avoid shared memory issues/race conditions
* 
* Wrapper for PPP_unstuff, essentially
* 
* INPUTS: 
*	new_byte:
*	payload_buffer_size:
*	input_buffer_size
* MEMORY/HELPER VARIABLES (PBR)
*	input_buffer:
*	bidx:
* OUTPUTS:
*	payload_buffer: result of PPP unstuffing
*	returns: size of the payload buffer. valid 
*/
int parse_PPP_stream(uint8_t new_byte, ppp_buffer_t * unstuffed_buffer, ppp_buffer_t * input_buffer)
{
	if (input_buffer->length < input_buffer->size)
	{
		input_buffer->buf[input_buffer->length++] = new_byte;
	}
	else
	{
		input_buffer->length = 0;	//overwrite everything in the buffer in the buffer overflow case. can change to circular buffer in future implementations
		return 0;
	}
	if (new_byte == FRAME_CHAR)
	{
		int pld_size = PPP_unstuff(unstuffed_buffer, input_buffer);	//important to use buffer idx as input buffer size, because the section of the input buffer after buffer idx might contain partial data frames
		input_buffer->length = 0;
		input_buffer->buf[input_buffer->length++] = new_byte;
		return pld_size;	//in the case that it is zero, return 0 anyway
	}
	return 0;
}
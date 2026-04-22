#include "bytestream.h"

/*
Basic check function
*/
static int check_buf(bytestream_t * buf)
{
	if(buf == NULL)
	{
		return BYTESTREAM_ERROR_NULLPTR;
	}
	else if (buf->buf == NULL)
	{
		return BYTESTREAM_ERROR_NULLPTR;
	}
	else if(buf->size == 0)
	{
		return BYTESTREAM_ERROR_EMPTYBUFFER;
	}
	else if(buf->len > buf->size)
	{
		return BYTESTREAM_ERROR_OVERRUN;
	}
	return BYTESTREAM_SUCCESS;
}



/*
Unified logic for delimiter character based stream decoding
*/
int bytestream(unsigned char new_byte, bytestream_t * input, unsigned char delimiter)
{
	int rc = check_buf(input);
	if(rc != BYTESTREAM_SUCCESS)
	{
		return rc;
	}

	if(input->pos >= input->size)
	{
		input->pos = 0;
		return BYTESTREAM_ERROR_OVERRUN;
	}

	input->buf[input->pos++] = new_byte;

	if(new_byte == delimiter)
	{
		input->len = input->pos;
		input->pos = 0;
		return BYTESTREAM_SUCCESS;	//mark "input" as valid for decode
	}
	return BYTESTREAM_IN_PROGRESS;
}


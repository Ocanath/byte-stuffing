#include "bytestream.h"

/*
Basic check function
*/
int check_buf(bytestream_buf_t * buf)
{
	if(buf == NULL)
	{
		return STREAM_ERROR_NULLPTR;
	}
	else if (buf->buf == NULL)
	{
		return STREAM_ERROR_NULLPTR;
	}
	else if(buf->size == 0)
	{
		return STREAM_ERROR_EMPTYBUFFER;
	}
	else if(buf->len > buf->size)
	{
		return STREAM_ERROR_OVERRUN;
	}
	return STREAM_SUCCESS;
}

/*
Unified logic for delimiter character based stream decoding
*/
int bytestream(unsigned char new_byte, bytestream_buf_t * input, unsigned char delimiter)
{
	int rc = check_buf(input);
	if(rc != STREAM_SUCCESS)
	{
		return rc;
	}

	if(input->pos >= input->size)
	{
		input->pos = 0;
		return STREAM_ERROR_OVERRUN;
	}

	input->buf[input->pos++] = new_byte;

	if(new_byte == delimiter)
	{
		input->len = input->pos;
		input->pos = 0;
		return STREAM_SUCCESS;	//mark "input" as valid for decode
	}
	return STREAM_IN_PROGRESS;
}





int bytestream_dual_delimiter(unsigned char new_byte, bytestream_buf_t * input, unsigned char delimiter)
{
	int rc = check_buf(input);
	if(rc != STREAM_SUCCESS)
	{
		return rc;
	}

	if(input->pos >= input->size)
	{
		input->pos = 0;
		return STREAM_ERROR_OVERRUN;
	}

	input->buf[input->pos++] = new_byte;

	if(new_byte == delimiter)
	{
		input->len = input->pos;
		input->pos = 0;
		input->buf[input->pos++] = new_byte;
		return STREAM_SUCCESS;	//mark "input" as valid for decode
	}
	return STREAM_IN_PROGRESS;
}
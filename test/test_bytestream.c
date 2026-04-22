#include "unity.h"
#include "PPP.h"
#include "cobs.h"
#include "bytestream.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>


void test_bytestream_PPP_multiple_strays(void)
{
	unsigned char example_data[128] = {0};
	size_t len = 0;
	example_data[len++] = 0;
	example_data[len++] = '~';
	example_data[len++] = 1;
	example_data[len++] = 2;
	example_data[len++] = 3;
	example_data[len++] = 4;
	example_data[len++] = 5;
	example_data[len++] = '~';
	example_data[len++] = 0;
	example_data[len++] = '~';
	example_data[len++] = 6;
	example_data[len++] = 7;
	example_data[len++] = 8;
	example_data[len++] = '~';
	example_data[len++] = '~';
	example_data[len++] = '~';
	example_data[len++] = 7;
	example_data[len++] = 8;
	example_data[len++] = '~';



	unsigned char stream_mem[32] = {0};
	bytestream_buf_t stream = {stream_mem, sizeof(stream_mem), 0, 0};

	unsigned char unstuff_mem[sizeof(stream_mem)*2+2] = {0};
	ppp_buffer_t unstuffed = {unstuff_mem, sizeof(unstuff_mem), 0};

	int found_frame_count = 0;
	int frame_size = 0;
	for(size_t i = 0; i < sizeof(example_data); i++)
	{
		uint8_t newbyte = example_data[i];
		bool expect_overrun = false;
		if(stream.pos >= stream.size)
		{
			expect_overrun = true;
		}		
		int rc = bytestream_dual_delimiter(newbyte, &stream, '~');
		if(expect_overrun)
		{
			TEST_ASSERT_EQUAL(BYTESTREAM_ERROR_OVERRUN, rc);
			TEST_ASSERT_EQUAL(0, stream.pos);
		}
		else
		{
			TEST_ASSERT(rc == BYTESTREAM_SUCCESS || rc == BYTESTREAM_IN_PROGRESS);
		}
		if(rc == BYTESTREAM_SUCCESS)
		{
			ppp_buffer_t stuffed = {stream.buf, stream.size, stream.len};
			int unstuffed_size = PPP_unstuff(&unstuffed, &stuffed);

			if(unstuffed_size != 0)
			{
				frame_size = unstuffed_size;
				found_frame_count++;
				if(found_frame_count == 1)
				{
					TEST_ASSERT_EQUAL(5, unstuffed.length);
					size_t unstuffed_idx = 0;
					for(int i = 1; i <= 5; i++)
					{
						TEST_ASSERT_EQUAL(i, unstuffed.buf[unstuffed_idx++]);
					}
				}
				else if(found_frame_count == 2)
				{
					TEST_ASSERT_EQUAL(1, frame_size);
					TEST_ASSERT_EQUAL(frame_size, unstuffed.length);
					TEST_ASSERT_EQUAL(0, unstuffed.buf[0]);
				}
				else if(found_frame_count == 3)
				{
					TEST_ASSERT_EQUAL(3, unstuffed.length);
					size_t unstuffed_idx = 0;
					for(int i = 6; i <= 8; i++)
					{
						TEST_ASSERT_EQUAL(i, unstuffed.buf[unstuffed_idx++]);
					}
				}
			}
			
		}
	}
	TEST_ASSERT_EQUAL(4, found_frame_count);
}




void test_bytestream_PPP_multiple_strays_no_dual_delimiter(void)
{
	unsigned char example_data[128] = {0};
	size_t len = 0;
	example_data[len++] = 0;
	example_data[len++] = '~';
	example_data[len++] = 1;
	example_data[len++] = 2;
	example_data[len++] = 3;
	example_data[len++] = 4;
	example_data[len++] = 5;
	example_data[len++] = '~';
	example_data[len++] = 0;
	example_data[len++] = '~';
	example_data[len++] = 6;
	example_data[len++] = 7;
	example_data[len++] = 8;
	example_data[len++] = '~';
	example_data[len++] = '~';
	example_data[len++] = '~';
	example_data[len++] = 7;
	example_data[len++] = 8;
	example_data[len++] = '~';



	unsigned char stream_mem[32] = {0};
	stream_mem[0] = '~';
	bytestream_buf_t stream = {&stream_mem[1], sizeof(stream_mem) - 1, 0, 0};

	unsigned char unstuff_mem[sizeof(stream_mem)*2+2] = {0};
	ppp_buffer_t unstuffed = {unstuff_mem, sizeof(unstuff_mem), 0};

	int found_frame_count = 0;
	int frame_size = 0;
	for(size_t i = 0; i < sizeof(example_data); i++)
	{
		uint8_t newbyte = example_data[i];
		bool expect_overrun = false;
		if(stream.pos >= stream.size)
		{
			expect_overrun = true;
		}		
		int rc = bytestream(newbyte, &stream, '~');
		if(expect_overrun)
		{
			TEST_ASSERT_EQUAL(BYTESTREAM_ERROR_OVERRUN, rc);
			TEST_ASSERT_EQUAL(0, stream.pos);
		}
		else
		{
			TEST_ASSERT(rc == BYTESTREAM_SUCCESS || rc == BYTESTREAM_IN_PROGRESS);
		}
		if(rc == BYTESTREAM_SUCCESS)
		{
			ppp_buffer_t stuffed = {stream_mem, sizeof(stream_mem), stream.len + 1};
			int unstuffed_size = PPP_unstuff(&unstuffed, &stuffed);

			if(unstuffed_size != 0)
			{
				frame_size = unstuffed_size;
				found_frame_count++;
				if(found_frame_count == 1)
				{
					TEST_ASSERT_EQUAL(1, unstuffed.length);
					TEST_ASSERT_EQUAL(0, unstuffed.buf[0]);
				}
				if(found_frame_count == 2)
				{
					TEST_ASSERT_EQUAL(5, unstuffed.length);
					size_t unstuffed_idx = 0;
					for(int i = 1; i <= 5; i++)
					{
						TEST_ASSERT_EQUAL(i, unstuffed.buf[unstuffed_idx++]);
					}
				}
				else if(found_frame_count == 3)
				{
					TEST_ASSERT_EQUAL(1, frame_size);
					TEST_ASSERT_EQUAL(frame_size, unstuffed.length);
					TEST_ASSERT_EQUAL(0, unstuffed.buf[0]);
				}
				else if(found_frame_count == 4)
				{
					TEST_ASSERT_EQUAL(3, unstuffed.length);
					size_t unstuffed_idx = 0;
					for(int i = 6; i <= 8; i++)
					{
						TEST_ASSERT_EQUAL(i, unstuffed.buf[unstuffed_idx++]);
					}
				}
				else if(found_frame_count == 5)
				{
					TEST_ASSERT_EQUAL(2, unstuffed.length);
					size_t unstuffed_idx = 0;
					for(int i = 7; i <= 8; i++)
					{
						TEST_ASSERT_EQUAL(i, unstuffed.buf[unstuffed_idx++]);
					}
				}

			}
			
		}
	}
	TEST_ASSERT_EQUAL(5, found_frame_count);
}



void test_bytestream_cobs_multiple_strays(void)
{
	unsigned char example_data[128] = {0};
	size_t example_len = 0;
	size_t start_pos = 1;

	//set up a byte stream. a little more complex because cobs is not as easy to manually encode
	{
		cobs_buf_t data_alias = {&example_data[start_pos], sizeof(example_data) - start_pos, 0, COBS_DECODED };
		data_alias.buf[data_alias.length++] = 1;
		data_alias.buf[data_alias.length++] = 2;
		data_alias.buf[data_alias.length++] = 3;
		data_alias.buf[data_alias.length++] = 4;
		data_alias.buf[data_alias.length++] = 5;
		int rc = cobs_encode_single_buffer(&data_alias);
		TEST_ASSERT(rc == 0);
		example_len = start_pos + data_alias.length;
		start_pos = example_len;
	}
	{
		cobs_buf_t data_alias = {&example_data[start_pos], sizeof(example_data) - start_pos, 0, COBS_DECODED };
		data_alias.buf[data_alias.length++] = 0;
		int rc = cobs_encode_single_buffer(&data_alias);
		TEST_ASSERT(rc == 0);
		example_len = start_pos + data_alias.length;
		start_pos = example_len;
	}
	{
		cobs_buf_t data_alias = {&example_data[start_pos], sizeof(example_data) - start_pos, 0, COBS_DECODED };
		data_alias.buf[data_alias.length++] = 6;
		data_alias.buf[data_alias.length++] = 7;
		data_alias.buf[data_alias.length++] = 8;
		int rc = cobs_encode_single_buffer(&data_alias);
		TEST_ASSERT(rc == 0);
		example_len = start_pos + data_alias.length;
		start_pos = example_len;
	}
	{
		cobs_buf_t data_alias = {&example_data[start_pos], sizeof(example_data) - start_pos, 0, COBS_DECODED };
		data_alias.buf[data_alias.length++] = 0;
		data_alias.buf[data_alias.length++] = 0;
		example_len = start_pos + data_alias.length;
		start_pos = example_len;
	}	
	{
		cobs_buf_t data_alias = {&example_data[start_pos], sizeof(example_data) - start_pos, 0, COBS_DECODED };
		data_alias.buf[data_alias.length++] = 7;
		data_alias.buf[data_alias.length++] = 8;
		int rc = cobs_encode_single_buffer(&data_alias);
		TEST_ASSERT(rc == 0);
		example_len = start_pos + data_alias.length;
		start_pos = example_len;
	}


	unsigned char stream_mem[32] = {0};
	bytestream_buf_t stream = {stream_mem, sizeof(stream_mem), 0, 0};

	unsigned char decode_mem[sizeof(stream_mem)+2] = {0};
	cobs_buf_t decoded = {decode_mem, sizeof(decode_mem), 0, COBS_DECODED};

	int found_frame_count = 0;
	int frame_size = 0;
	for(size_t i = 0; i < sizeof(example_data); i++)
	{
		uint8_t newbyte = example_data[i];
		bool expect_overrun = false;
		if(stream.pos >= stream.size)
		{
			expect_overrun = true;
		}		
		int bsrc = bytestream(newbyte, &stream, 0);
		if(expect_overrun)
		{
			TEST_ASSERT_EQUAL(BYTESTREAM_ERROR_OVERRUN, bsrc);
			TEST_ASSERT_EQUAL(0, stream.pos);
		}
		else
		{
			TEST_ASSERT(bsrc == BYTESTREAM_SUCCESS || bsrc == BYTESTREAM_IN_PROGRESS);
		}
		if(bsrc == BYTESTREAM_SUCCESS)
		{
			cobs_buf_t encoded = {stream.buf, stream.size, stream.len, COBS_ENCODED};
			int cobsrc = cobs_decode_double_buffer(&encoded, &decoded);
			if(cobsrc == COBS_SUCCESS && decoded.length > 0)
			{
				TEST_ASSERT_EQUAL(BYTESTREAM_SUCCESS, bsrc);
				frame_size = decoded.length;
				found_frame_count++;
				if(found_frame_count == 1)
				{
					TEST_ASSERT_EQUAL(5, decoded.length);
					size_t unstuffed_idx = 0;
					for(int i = 1; i <= 5; i++)
					{
						TEST_ASSERT_EQUAL(i, decoded.buf[unstuffed_idx++]);
					}
				}
				else if(found_frame_count == 2)
				{
					TEST_ASSERT_EQUAL(1, frame_size);
					TEST_ASSERT_EQUAL(frame_size, decoded.length);
					TEST_ASSERT_EQUAL(0, decoded.buf[0]);
				}
				else if(found_frame_count == 3)
				{
					TEST_ASSERT_EQUAL(3, decoded.length);
					size_t unstuffed_idx = 0;
					for(int i = 6; i <= 8; i++)
					{
						TEST_ASSERT_EQUAL(i, decoded.buf[unstuffed_idx++]);
					}
				}
			}
			
		}
	}
	TEST_ASSERT_EQUAL(4, found_frame_count);
}


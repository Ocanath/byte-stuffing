#include "unity.h"
#include "cobs.h"
#include <stdlib.h>
#include <time.h>

void setUp(void)
{

}

void tearDown(void)
{

}

void test_cobs_encode_null_pointer(void)
{
    int rc = cobs_encode(NULL);
    TEST_ASSERT_EQUAL(rc, -1);
}

void test_cobs_encode_small_buffer(void)
{
	unsigned char msg_buf[] = {0};
	cobs_buf_t msg = 
	{
		.buf = msg_buf,
		.size = sizeof(msg_buf),
		.length = 1,
		.state = COBS_DECODED
	};
	int rc = cobs_encode(&msg);
	TEST_ASSERT_EQUAL(-1, rc);	//should return -1 due to the buffer being too small. 
}

void test_cobs_encode(void)
{
	{
		unsigned char msg_buf[3] = {0x0};	
		cobs_buf_t msg = 
		{
			.buf = msg_buf,
			.size = sizeof(msg_buf),
			.length = 1,
			.state = COBS_DECODED
		};
		int rc = cobs_encode(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(1, msg.buf[0]);
		TEST_ASSERT_EQUAL(1, msg.buf[1]);
		TEST_ASSERT_EQUAL(0, msg.buf[2]);
		TEST_ASSERT_EQUAL(3, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.state);
	}
	{
		unsigned char msg_buf[253] = {};
		msg_buf[0] = 0;
		msg_buf[1] = 0;
		cobs_buf_t msg = 
		{
			.buf = msg_buf,
			.size = sizeof(msg_buf),
			.length = 2,
			.state = COBS_DECODED
		};
		int rc = cobs_encode(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(1, msg.buf[0]);
		TEST_ASSERT_EQUAL(1, msg.buf[1]);
		TEST_ASSERT_EQUAL(1, msg.buf[2]);
		TEST_ASSERT_EQUAL(0, msg.buf[3]);
		TEST_ASSERT_EQUAL(4, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.state);
	}
	{
		unsigned char msg_buf[] = {
			0,
			11,
			0,
			0,
			0
		};
		cobs_buf_t msg = {
			.buf=  msg_buf,
			.size = sizeof(msg_buf),
			.length = sizeof(msg_buf)-2,
			.state = COBS_DECODED
		};
		int rc = cobs_encode(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(1, msg.buf[0]);
		TEST_ASSERT_EQUAL(2, msg.buf[1]);
		TEST_ASSERT_EQUAL(11, msg.buf[2]);
		TEST_ASSERT_EQUAL(1, msg.buf[3]);
		TEST_ASSERT_EQUAL(0, msg.buf[4]);
		TEST_ASSERT_EQUAL(5, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.state);
	}
	{
        int bidx = 0;
		unsigned char msg_buf[4+2] = {};
		msg_buf[bidx++] = 11;
		msg_buf[bidx++] = 22;
        msg_buf[bidx++] = 00;
        msg_buf[bidx++] = 33;
		cobs_buf_t msg = 
		{
			.buf = msg_buf,
			.size = sizeof(msg_buf),
			.length = bidx,
			.state = COBS_DECODED
		};
		int rc = cobs_encode(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(3, msg.buf[0]);
		TEST_ASSERT_EQUAL(11, msg.buf[1]);
		TEST_ASSERT_EQUAL(22, msg.buf[2]);
		TEST_ASSERT_EQUAL(02, msg.buf[3]);
        TEST_ASSERT_EQUAL(33, msg.buf[4]);
        TEST_ASSERT_EQUAL(0, msg.buf[5]);
		TEST_ASSERT_EQUAL(6, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.state);
	}
	{
        int bidx = 0;
		unsigned char msg_buf[4+2] = {};
		msg_buf[bidx++] = 11;
		msg_buf[bidx++] = 22;
        msg_buf[bidx++] = 33;
        msg_buf[bidx++] = 44;
		cobs_buf_t msg = 
		{
			.buf = msg_buf,
			.size = sizeof(msg_buf),
			.length = bidx,
			.state = COBS_DECODED
		};
		int rc = cobs_encode(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(5, msg.buf[0]);
		TEST_ASSERT_EQUAL(11, msg.buf[1]);
		TEST_ASSERT_EQUAL(22, msg.buf[2]);
		TEST_ASSERT_EQUAL(33, msg.buf[3]);
        TEST_ASSERT_EQUAL(44, msg.buf[4]);
        TEST_ASSERT_EQUAL(0, msg.buf[5]);
		TEST_ASSERT_EQUAL(6, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.state);
	}

    {
		unsigned char msg_buf[] = {
            11,
            0,
            0,
            0,
            0,
            0,
        };
		cobs_buf_t msg = 
		{
			.buf = msg_buf,
			.size = sizeof(msg_buf),
			.length = sizeof(msg_buf) - 2, //last two bytes are zero for delimiter and to make room for prepended pointer
			.state = COBS_DECODED
		};
		int rc = cobs_encode(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(2, msg.buf[0]);
		TEST_ASSERT_EQUAL(11, msg.buf[1]);
		TEST_ASSERT_EQUAL(1, msg.buf[2]);
		TEST_ASSERT_EQUAL(1, msg.buf[3]);
        TEST_ASSERT_EQUAL(1, msg.buf[4]);
        TEST_ASSERT_EQUAL(0, msg.buf[5]);
		TEST_ASSERT_EQUAL(6, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.state);
	}
	{
		unsigned char msg_buf[256] = {0};	
		for(int i = 0; i < 254; i++)		//test case: max length nonzero before needing to handle pointer overflow
		{
			msg_buf[i] = i+1;
		}
		cobs_buf_t msg = {
			.buf = msg_buf,
			.size = sizeof(msg_buf),
			.length = 254,
			.state = COBS_DECODED
		};
		TEST_ASSERT_EQUAL(0xFE, msg.buf[msg.length-1]);
		int rc = cobs_encode(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(256, msg.length);
		TEST_ASSERT_EQUAL(0xFF, msg.buf[0]);
		for(int i = 1; i < 0xFE; i++)
		{
			TEST_ASSERT_EQUAL(i, msg.buf[i]);
		}
		TEST_ASSERT_EQUAL(0, msg.buf[msg.length-1]);
	}
	{
		unsigned char msg_buf[257] = {0};	
		/*similar to previous test, in that this is the max block size,
		 just shifted in a message which has smaller blocks in the beginning.
		  In this case a single block of size 1 (a zero byte)*/
		for(int i = 0; i < 255; i++)		
		{
			msg_buf[i] = i;
		}
		cobs_buf_t msg = {
			.buf = msg_buf,
			.size = sizeof(msg_buf),
			.length = 255,
			.state = COBS_DECODED
		};
		TEST_ASSERT_EQUAL(0xFE, msg.buf[msg.length-1]);
		int rc = cobs_encode(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(257, msg.length);
		TEST_ASSERT_EQUAL(0x1, msg.buf[0]);
		TEST_ASSERT_EQUAL(0xFF, msg.buf[1]);
		for(int i = 2; i < msg.length-1; i++)
		{
			TEST_ASSERT_EQUAL(i-1, msg.buf[i]);
		}
		TEST_ASSERT_EQUAL(0, msg.buf[msg.length-1]);
	}

}
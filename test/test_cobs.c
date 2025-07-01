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

}
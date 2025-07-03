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
    int rc = cobs_encode_single_buffer(NULL);
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
		.encoded_state = COBS_DECODED
	};
	int rc = cobs_encode_single_buffer(&msg);
	TEST_ASSERT_EQUAL(COBS_ERROR_SIZE, rc);	//should return -1 due to the buffer being too small. 
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
			.encoded_state = COBS_DECODED
		};
		int rc = cobs_encode_single_buffer(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(1, msg.buf[0]);
		TEST_ASSERT_EQUAL(1, msg.buf[1]);
		TEST_ASSERT_EQUAL(0, msg.buf[2]);
		TEST_ASSERT_EQUAL(3, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.encoded_state);
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
			.encoded_state = COBS_DECODED
		};
		int rc = cobs_encode_single_buffer(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(1, msg.buf[0]);
		TEST_ASSERT_EQUAL(1, msg.buf[1]);
		TEST_ASSERT_EQUAL(1, msg.buf[2]);
		TEST_ASSERT_EQUAL(0, msg.buf[3]);
		TEST_ASSERT_EQUAL(4, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.encoded_state);
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
			.encoded_state = COBS_DECODED
		};
		int rc = cobs_encode_single_buffer(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(1, msg.buf[0]);
		TEST_ASSERT_EQUAL(2, msg.buf[1]);
		TEST_ASSERT_EQUAL(11, msg.buf[2]);
		TEST_ASSERT_EQUAL(1, msg.buf[3]);
		TEST_ASSERT_EQUAL(0, msg.buf[4]);
		TEST_ASSERT_EQUAL(5, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.encoded_state);
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
			.encoded_state = COBS_DECODED
		};
		int rc = cobs_encode_single_buffer(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(3, msg.buf[0]);
		TEST_ASSERT_EQUAL(11, msg.buf[1]);
		TEST_ASSERT_EQUAL(22, msg.buf[2]);
		TEST_ASSERT_EQUAL(02, msg.buf[3]);
        TEST_ASSERT_EQUAL(33, msg.buf[4]);
        TEST_ASSERT_EQUAL(0, msg.buf[5]);
		TEST_ASSERT_EQUAL(6, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.encoded_state);
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
			.encoded_state = COBS_DECODED
		};
		int rc = cobs_encode_single_buffer(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(5, msg.buf[0]);
		TEST_ASSERT_EQUAL(11, msg.buf[1]);
		TEST_ASSERT_EQUAL(22, msg.buf[2]);
		TEST_ASSERT_EQUAL(33, msg.buf[3]);
        TEST_ASSERT_EQUAL(44, msg.buf[4]);
        TEST_ASSERT_EQUAL(0, msg.buf[5]);
		TEST_ASSERT_EQUAL(6, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.encoded_state);
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
			.encoded_state = COBS_DECODED
		};
		int rc = cobs_encode_single_buffer(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(2, msg.buf[0]);
		TEST_ASSERT_EQUAL(11, msg.buf[1]);
		TEST_ASSERT_EQUAL(1, msg.buf[2]);
		TEST_ASSERT_EQUAL(1, msg.buf[3]);
        TEST_ASSERT_EQUAL(1, msg.buf[4]);
        TEST_ASSERT_EQUAL(0, msg.buf[5]);
		TEST_ASSERT_EQUAL(6, msg.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, msg.encoded_state);
	}
	{
		unsigned char msg_buf[257] = {0};	
		for(int i = 0; i < 254; i++)		//test case: max length nonzero before needing to handle pointer overflow
		{
			msg_buf[i] = i+1;
		}
		cobs_buf_t msg = {
			.buf = msg_buf,
			.size = sizeof(msg_buf),
			.length = 254,
			.encoded_state = COBS_DECODED
		};
		TEST_ASSERT_EQUAL(0xFE, msg.buf[msg.length-1]);
		int rc = cobs_encode_single_buffer(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(256, msg.length);
		TEST_ASSERT_EQUAL(0xFF, msg.buf[0]);
		for(int i = 1; i < 0xFE; i++)
		{
			TEST_ASSERT_EQUAL(i, msg.buf[i]);
		}
		TEST_ASSERT_EQUAL(0, msg.buf[msg.length-1]);
	}
	{ //test case number 9 on wikipedia, demonstrating proper handling of the maximum block size of 254
		unsigned char msg_buf[258] = {0};	
		/*similar to previous test, in that this is the max block size,
		 just shifted in a message which has smaller blocks in the beginning.
		  In this case a single block of size 1 (a zero byte)*/
		for(int i = 0; i < 255; i++)		
		{
			msg_buf[i] = i+1;
		}
		for(int i = 255; i < 258; i++)
		{
			msg_buf[i] = 0xDE;
		}
		cobs_buf_t msg = 
		{
			.buf = msg_buf,
			.size = sizeof(msg_buf),
			.length = 255,
			.encoded_state = COBS_DECODED
		};
		
		printf("Original unencoded: ");
		for(int i = 0; i < 3; i++)
		{
			printf("%.2X ", msg.buf[i]);
		}
		printf(" ... ");
		for(int i = msg.length-3; i < msg.length; i++)
		{
			printf("%.2X ", msg.buf[i]);
		}
		printf("\r\n");

		int rc = cobs_encode_single_buffer(&msg);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(258, msg.length);

		printf("Encoded: ");
		for(int i = 0; i < 4; i++)
		{
			printf("%.2X ", msg.buf[i]);
		}
		printf(" ... ");
		for(int i = msg.length-5; i < msg.length; i++)
		{
			printf("%.2X ", msg.buf[i]);
		}
		printf("\r\n");


		TEST_ASSERT_EQUAL(0xFF, msg.buf[0]);
		for(int i = 1; i <= 0xFE; i++)
		{
			TEST_ASSERT_EQUAL(i, msg.buf[i]);
		}
		TEST_ASSERT_EQUAL(0x2, msg.buf[msg.length-3]);
		TEST_ASSERT_EQUAL(0xFF, msg.buf[msg.length-2]);
		TEST_ASSERT_EQUAL(0, msg.buf[msg.length-1]);
	}
	{//test overflow
		unsigned char large_msg_buf[1024] = {};
		for(int i = 0; i < sizeof(large_msg_buf)-6; i++)
		{
			large_msg_buf[i] = (i % 512) + 1;
		}
		cobs_buf_t msg = {
			.buf = large_msg_buf,
			.size = sizeof(large_msg_buf),
			.length = sizeof(large_msg_buf)-6,
			.encoded_state = COBS_DECODED
		};
		int rc = cobs_encode_single_buffer(&msg);
		TEST_ASSERT_EQUAL(0, rc);

	}
}

void printf_cobs_buf(cobs_buf_t * pcb)
{
	int i = 0;			
	for(i = 0; i < pcb->length - 1 && i < pcb->size; i++)
	{
		printf("0x%.2X, ", pcb->buf[i]);
	}
	printf("0x%.2X\r\n", pcb->buf[i]);
}

void test_cobs_decode_double_buffer(void)
{
	{
        int bidx = 0;
		unsigned char encode_buf[4+2] = {};
		encode_buf[bidx++] = 11;
		encode_buf[bidx++] = 22;
        encode_buf[bidx++] = 00;
        encode_buf[bidx++] = 33;
		cobs_buf_t encode = 
		{
			.buf = encode_buf,
			.size = sizeof(encode_buf),
			.length = bidx,
			.encoded_state = COBS_DECODED
		};
		int rc = cobs_encode_single_buffer(&encode);
		TEST_ASSERT_EQUAL(0, rc);
		TEST_ASSERT_EQUAL(3, encode.buf[0]);
		TEST_ASSERT_EQUAL(11, encode.buf[1]);
		TEST_ASSERT_EQUAL(22, encode.buf[2]);
		TEST_ASSERT_EQUAL(02, encode.buf[3]);
        TEST_ASSERT_EQUAL(33, encode.buf[4]);
        TEST_ASSERT_EQUAL(0, encode.buf[5]);
		TEST_ASSERT_EQUAL(6, encode.length);
		TEST_ASSERT_EQUAL(COBS_ENCODED, encode.encoded_state);

		unsigned char decode_buf[sizeof(encode_buf)];
		cobs_buf_t decode = 
		{
			.buf = decode_buf,
			.size = sizeof(decode_buf),
			.length = 0,
			.encoded_state = COBS_DECODED	//not yet decoded but it should read perma decode regardless. length marks it as empty
		};
		rc = cobs_decode_double_buffer(&encode, &decode);
		TEST_ASSERT_EQUAL(0,rc);
		TEST_ASSERT_EQUAL(11, decode.buf[0]);
		TEST_ASSERT_EQUAL(22, decode.buf[1]);
		TEST_ASSERT_EQUAL(00, decode.buf[2]);
		TEST_ASSERT_EQUAL(33, decode.buf[3]);
		TEST_ASSERT_EQUAL(4, decode.length);
		TEST_ASSERT_EQUAL(COBS_DECODED, decode.encoded_state);
	}



	{
		/*Begin new test message cases. Add for more coverage */
		const unsigned char msg1[] = {11, 0, 0, 0};
		const unsigned char msg2[] = {11, 12, 13, 0};
		const unsigned char msg3[] = {11, 0, 12, 13};
		const unsigned char msg4[] = {11, 0, 0, 13,15,16};
		const unsigned char msg5[] = {11, 0, 0, 0,1,7,16,16,0,0,0,0,1,0,0,1,0,1,0,1,0};
		const unsigned char msg6[] = {11, 01, 2, 255,255,255,255,255,255,0};
		const unsigned char msg7[] = {11, 255, 0, 255, 0, 255, 0, 255};
		const cobs_buf_t messages[] = {	
			{
				.buf = (unsigned char * )&msg1,
				.size = sizeof(msg1),
				.length = sizeof(msg1),
				.encoded_state = COBS_DECODED
			},
			{
				.buf = (unsigned char * )&msg2,
				.size = sizeof(msg2),
				.length = sizeof(msg2),
				.encoded_state = COBS_DECODED
			},
			{
				.buf = (unsigned char * )&msg3,
				.size = sizeof(msg3),
				.length = sizeof(msg3),
				.encoded_state = COBS_DECODED
			},
			{
				.buf = (unsigned char * )&msg4,
				.size = sizeof(msg4),
				.length = sizeof(msg4),
				.encoded_state = COBS_DECODED
			},
			{
				.buf = (unsigned char * )&msg5,
				.size = sizeof(msg5),
				.length = sizeof(msg5),
				.encoded_state = COBS_DECODED
			},
			{
				.buf = (unsigned char * )&msg6,
				.size = sizeof(msg6),
				.length = sizeof(msg6),
				.encoded_state = COBS_DECODED
			},
			{
				.buf = (unsigned char * )&msg7,
				.size = sizeof(msg7),
				.length = sizeof(msg7),
				.encoded_state = COBS_DECODED
			},
		};
		/*End new test message cases*/

		//set up the encode and decode buffers
		unsigned char encode_buf[64] = {};
		cobs_buf_t encode = {
			.buf = encode_buf,
			.size = sizeof(encode_buf),
			.length = 0
		};
		cobs_buf_t decode_buf[64] = {};
		cobs_buf_t decode = {
			.buf = decode_buf,
			.size = sizeof(decode_buf),
			.length = 0
		};
		const int num_messages = sizeof(messages)/sizeof(cobs_buf_t);
		for(int msgidx = 0; msgidx < num_messages; msgidx++)
		{
			for(int i = 0; i < messages[msgidx].length; i++)
			{
				encode.buf[i] = messages[msgidx].buf[i];
			}
			encode.length = messages[msgidx].length;
			int rc = cobs_encode_single_buffer(&encode);	//we assume correctness of this function for the purpose of this test, i.e. coverage from previous tests is sufficient for us to test decode against it
			rc = cobs_decode_double_buffer(&encode, &decode);
			TEST_ASSERT_EQUAL(messages[msgidx].length, decode.length);
			for(int i = 0; i < messages[msgidx].length; i++)
			{
				TEST_ASSERT_EQUAL(messages[msgidx].buf[i], decode.buf[i]);
			}
			// printf("original: ");
			// printf_cobs_buf(&messages[msgidx]);
			// printf("encoded: ");
			// printf_cobs_buf(&encode);
			// printf("decoded: ");
			// printf_cobs_buf(&decode);
		}
	}
}


void test_cobs_decode_double_buffer_large_message(void)
{
	unsigned char msg8[1024] = {};
	for(int i = 0; i < sizeof(msg8); i++)
	{
		msg8[i] = (i % 255) + 1;	//fill the whole thing with nonzero bytes
	}
	for(int i = 0; i < sizeof(msg8); i++)
	{
		TEST_ASSERT_NOT_EQUAL(0, msg8[i]);
	}

	//set up the encode and decode buffers
	unsigned char encode_buf[2048] = {};
	cobs_buf_t encode = {
		.buf = encode_buf,
		.size = sizeof(encode_buf),
		.length = 0
	};
	cobs_buf_t decode_buf[2048] = {};
	cobs_buf_t decode = {
		.buf = decode_buf,
		.size = sizeof(decode_buf),
		.length = 0
	};

	//encode the message
	for(int i = 0; i < sizeof(msg8); i++)
	{
		encode.buf[i] = msg8[i];
	}
	encode.length = sizeof(msg8);
	//encode might be wrong for large buffers
	int rc = cobs_encode_single_buffer(&encode);	//we assume correctness of this function for the purpose of this test, i.e. coverage from previous tests is sufficient for us to test decode against it
	TEST_ASSERT_EQUAL(0, rc);
	rc = cobs_decode_double_buffer(&encode, &decode);
	for(int i = 0; i < sizeof(msg8); i++)
	{
		if(msg8[i] != decode.buf[i])
		{
			printf("bad idx is %d\r\n", i);
		}
		TEST_ASSERT_EQUAL(msg8[i], decode.buf[i]);
	}
	TEST_ASSERT_EQUAL(sizeof(msg8), decode.length);
}

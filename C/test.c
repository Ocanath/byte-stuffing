#include <stdio.h>
#include "PPP.h"
#include <stdlib.h>
#include <time.h>

#define PAYLOAD_SIZE 8
#define BUFFER_SIZE 128		//Note: The minimum buffersize should be max payload size * 2 + 2. Likewise, the maximum payload is (buffer_size-2)/2


uint8_t gl_stuffing_buffer[BUFFER_SIZE] = {0};
uint8_t gl_unstuffing_buffer[BUFFER_SIZE] = {0};

void print_hex_buffer(uint8_t * arr, int size)
{
	printf("0x");
	for(int i = 0; i < size; i++)
	{
		if(i < size-1)
			printf("% 0.2X ", arr[i]);
		else
			printf("%0.2X", arr[i]);
	}
}



void main()
{
	srand(time(NULL));

	buffer_t stuffed_buffer = { .buf = gl_stuffing_buffer, .size = BUFFER_SIZE, .length = 0 };
	buffer_t input_buffer = { .buf = gl_unstuffing_buffer, .size = BUFFER_SIZE, .length = 0 };

	uint8_t example_data[PAYLOAD_SIZE] = { 0 };
	for (int i = 0; i < PAYLOAD_SIZE; i++)
	{
		example_data[i] = rand() % 0x100;
	}
	buffer_t unstuffed_buffer = { .buf = example_data, .size = PAYLOAD_SIZE, .length = 0 };
	printf("Original Data: ");
	print_hex_buffer(example_data, PAYLOAD_SIZE);
	printf("\r\n");
	printf("Stuffed data: ");
	int stuffed_size = PPP_stuff(&unstuffed_buffer, &stuffed_buffer);
	print_hex_buffer(gl_stuffing_buffer, stuffed_size);
	printf("\r\nsize is %d\r\n", stuffed_size);

	printf("Second payload: ");
	for (int i = 0; i < 3; i++)
		example_data[i] = rand() % 0x100;
	example_data[3] = FRAME_CHAR;
	print_hex_buffer(example_data, 4);
	printf("\r\n");
	int new_stuffed_size = PPP_stuff(&unstuffed_buffer, &stuffed_buffer);
	print_hex_buffer(&gl_stuffing_buffer[stuffed_size], new_stuffed_size);
	printf("\r\nsize is %d\r\n", new_stuffed_size);


	int bidx = 0;
	uint8_t pld_buf[64] = {0};
	for(int i = 0; i < BUFFER_SIZE; i++)	
	{
		uint8_t new_byte = gl_stuffing_buffer[i];	//This is emulating a new byte coming in from hardware
		
		
		int pld_size = parse_PPP_stream(new_byte, &unstuffed_buffer, &input_buffer);
		if(pld_size != 0)	//handle pld size nonzero case (valid data). i.e. copy it, etc.
		{
			printf("Unstuffed stream payload is: ");
			print_hex_buffer(pld_buf, pld_size);
			printf("\r\n");
		}
	}
	
}


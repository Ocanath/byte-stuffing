#include <stdio.h>
#include "PPP.h"
#include <stdlib.h>
#include <time.h>

#define PAYLOAD_SIZE 8
#define BUFFER_SIZE 128		//Note: The minimum buffersize should be max payload size * 2 + 2. Likewise, the maximum payload is (buffer_size-2)/2


uint8_t gl_stuffing_buffer[BUFFER_SIZE] = {0};


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

	uint8_t example_data[PAYLOAD_SIZE] = { 0 };
	for (int i = 0; i < PAYLOAD_SIZE; i++)
	{
		example_data[i] = rand() % 0x100;
	}
		

	printf("Original Data: ");
	print_hex_buffer(example_data, PAYLOAD_SIZE);
	printf("\r\n");
	printf("Stuffed data: ");
	int stuffed_size = PPP_stuff(example_data, PAYLOAD_SIZE, gl_stuffing_buffer, BUFFER_SIZE);
	print_hex_buffer(gl_stuffing_buffer, stuffed_size);
	printf("\r\nsize is %d\r\n", stuffed_size);

	printf("Unstuffed data: ");
	uint8_t pld_buf[64] = { 0 };//dummy buffer. For the purposes of an example, we're showing a working buffer for unstuff can be larger than the true payload size
	int unstuffed_size = PPP_unstuff(pld_buf, 64, gl_stuffing_buffer, BUFFER_SIZE);
	print_hex_buffer(pld_buf, unstuffed_size);
	printf("\r\n");
}


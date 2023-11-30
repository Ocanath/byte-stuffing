#include "cobs-stuffing.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define PAYLOAD_SIZE 8

/**/
void print_hex_buffer(uint8_t * arr, int size)
{
	for(int i = 0; i < size; i++)
	{
		if(i < size-1)
			printf("% 0.2X ", arr[i]);
		else
			printf("%0.2X", arr[i]);
	}
}

int check_size(uint8_t * buffer)
{
    int i = 0;
    for(i = 0; buffer[i] != 0; i++);
    return i;
}

void main(void)
{
    srand(time(NULL));
	uint8_t example_data[PAYLOAD_SIZE] = { 0 };
	for (int i = 0; i < PAYLOAD_SIZE; i++)
	{
		example_data[i] = rand() % 0x100;
	}
    example_data[0] = 0;
    example_data[2] = 0;
    example_data[5] = 0;
    example_data[7] = 0;

    uint8_t encodebuf[128] = {0};
    uint8_t decodebuf[128] = {0};
    printf("Original Data: ");
	print_hex_buffer(example_data, PAYLOAD_SIZE);
    printf("\r\n");
    
    int encoded_size = cobsEncode(example_data, PAYLOAD_SIZE, encodebuf);
    int checked_size = check_size(encodebuf);
    if(encoded_size == checked_size)
    {
        printf("New Data:   ");
        print_hex_buffer(encodebuf, encoded_size);
        printf(" 00 \r\n");
    }
    else
    {
        printf("mismatch: encoded size %d, checked size %d\r\n", encoded_size, checked_size);
    }
    int decoded_size = cobsDecode(encodebuf, encoded_size, decodebuf);
    printf("Decoded Data:  ");
    print_hex_buffer(decodebuf, decoded_size);
    printf("\r\n");
}

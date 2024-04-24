#include <stdio.h>
#include <math.h>
#include <vector>
#include "winserial.h"
#include "PPP.h"
#include "args-parsing.h"

#define PAYLOAD_SIZE 512
#define UNSTUFFING_BUFFER_SIZE (PAYLOAD_SIZE * 2 + 2)

//Screen dimension constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

static int gl_ppp_bidx = 0;
static uint8_t gl_ppp_payload_buffer[PAYLOAD_SIZE] = { 0 };	//buffer
static uint8_t gl_ppp_unstuffing_buffer[UNSTUFFING_BUFFER_SIZE] = { 0 };
static uint8_t gl_ser_readbuf[512] = { 0 };
static float gl_valdump[PAYLOAD_SIZE / sizeof(float)] = { 0 };


static uint8_t gl_ppp_tx_prestuffing_buffer[PAYLOAD_SIZE] = { 0 };
static uint8_t gl_ppp_stuffing_buffer[UNSTUFFING_BUFFER_SIZE] = { 0 };


/*
Generic hex checksum calculation.
TODO: use this in the psyonic API
*/
uint32_t fletchers_checksum32(uint32_t* arr, int size)
{
	int32_t checksum = 0;
	int32_t fchk = 0;
	for (int i = 0; i < size; i++)
	{
		checksum += (int32_t)arr[i];
		fchk += checksum;
	}
	return fchk;
}


/*
Generic hex checksum calculation.
TODO: use this in the psyonic API
*/
uint8_t get_checksum(uint8_t* arr, int size)
{

	int8_t checksum = 0;
	for (int i = 0; i < size; i++)
		checksum += (int8_t)arr[i];
	return -checksum;
}

/*
* Inputs:
*	input_buf: raw unstuffed data buffer
* Outputs:
*	parsed_data: floats, parsed from input buffer
* Returns: number of parsed values
*/
void parse_PPP_values(uint8_t* input_buf, int payload_size, float* parsed_data, int * parsed_data_size)
{
	uint32_t* pbu32 = (uint32_t*)(&input_buf[0]);
	int32_t* pbi32 = (int32_t*)(&input_buf[0]);
	int wordsize = payload_size / sizeof(uint32_t);
	int i = 0;
	for (i = 0; i < wordsize - 1; i++)
	{
		parsed_data[i] = ((float)pbi32[i])/4096.f;
		//printf("%d ", pbi32[i]);
	}
	//printf("\r\n");
	parsed_data[i] = ((float)pbu32[i]) / 1000.f;

	*parsed_data_size = wordsize;
}


int create_abh_pctl_buf(uint8_t addr, uint8_t format_option, float * fpos, uint8_t* output_buf)
{
	int bidx = 0;
	output_buf[bidx++] = addr;
	output_buf[bidx++] = 0x10 + format_option;
	for (int ch = 0; ch < 6; ch++)
	{
		int32_t fsc_i32 = (int32_t)(fpos[ch] * 32767.f / 150.f);
		if (fsc_i32 > 32767)
			fsc_i32 = 32767;
		else if (fsc_i32 < -32767)
			fsc_i32 = -32767;
		u32_fmt_t fmt;
		fmt.i16[0] = (int16_t)fsc_i32;
		for (int i = 0; i < sizeof(int16_t); i++)
		{
			output_buf[bidx++] = fmt.ui8[i];
		}
	}
	output_buf[bidx++] = get_checksum(output_buf, bidx);
	return bidx;
}


int main(int argc, char* args[])
{
	parse_args(argc, args, &gl_options);

	HANDLE serialport;
	char namestr[16] = { 0 };
	uint8_t found = 0;
	for (int i = 0; i < 255; i++)
	{
		int rl = sprintf_s(namestr, "\\\\.\\COM%d", i);
		int rc = connect_to_usb_serial(&serialport, namestr, gl_options.baud_rate);
		if (rc != 0)
		{
			printf("Connected to COM port %s successfully\r\n", namestr);
			found = 1;
			break;
		}
	}
	if (found == 0)
	{
		printf("No COM ports found\r\n");
	}

	float fpos[6] = { 15.f ,15.f, 15.f, 15.f, 15.f, -15.f };
	
	uint64_t tx_ts = 0;


	int total_replies = 0;
	int valid_reply_count = 0;
	printf("Starting comm analysis...\r\n");
	uint64_t start_tick = GetTickCount64();
	std::vector<uint8_t> readbuf;



	//WriteFile(serialport, gl_ppp_stuffing_buffer, stuffed_size, (LPDWORD)(&num_bytes_written), NULL);

	while (total_replies < 1000)
	{
		uint64_t tick = GetTickCount64() - start_tick;
		
		{
			LPDWORD num_bytes_read = 0;
			int rc = ReadFile(serialport, gl_ser_readbuf, 512, (LPDWORD)(&num_bytes_read), NULL);	//should be a DOUBLE BUFFER!
			for (int i = 0; i < (int)num_bytes_read; i++)
			{
				uint8_t new_byte = gl_ser_readbuf[i];
				readbuf.push_back(new_byte);
			}
			//printf("size=%d\r\n", readbuf.size());
			for (int i = 0; i < ((int)readbuf.size())-1; i++)
			{
				if (readbuf[i] == 0x55 && readbuf[i+1] == 0xCC)
				{
					
					//printf("%d\r\n", i);
					int startidx = i - 28;
					int16_t x[3] = { 0 };
					int16_t y[3] = { 0 };
					int16_t V[3] = { 0 };
					int16_t pixRes[3] = { 0 };
					int16_t* pArr[4] = { x, y, V, pixRes };
					if (startidx >= 0)
					{
						printf("%X%X%X%0.2X: ", readbuf[startidx], readbuf[startidx + 1], readbuf[startidx + 2], readbuf[startidx + 3]);
						int bufidx = startidx + 4;
						for (int target = 0; target < 3; target++)
						{
							for (int arridx = 0; arridx < 4; arridx++)
							{
								int32_t tmp = 0;
								uint8_t b1 = readbuf[bufidx];
								uint8_t b2 = readbuf[bufidx + 1]; 
								tmp = (int32_t)b1 + ((int32_t)b2 * 256);
								if (tmp < 0x8000)
									pArr[arridx][target] = (int16_t)(0 - tmp);
								else
									pArr[arridx][target] = (int16_t)(tmp - 0x8000);
								bufidx += 2;
							}
							printf("(%d,%d,%d) ", x[target], y[target], V[target]);
						}
						printf("\r\n");
						
					}
					readbuf.clear();
				}
			}
		}

	}
	uint64_t end_tick = GetTickCount64();
	printf("Received %d valid replies out of %d total\r\n", valid_reply_count, total_replies);
	double runtime = ((double)end_tick - (double)start_tick) / 1000.0;
	printf("Total runtime %f seconds\r\n", runtime);
	double replies = total_replies;
	printf("Average bandwidth across run %f frames per second\r\n", replies/runtime);
	
}

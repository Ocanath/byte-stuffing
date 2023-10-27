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
	uint64_t start_time = GetTickCount64();
	uint64_t tx_ts = 0;

	while (1)
	{
		uint64_t tick = GetTickCount64() - start_time;
		
		//if (tick - tx_ts > 1)
		{
			tx_ts = tick;

			float t = ((float)tick) * 0.001f;

			for (int ch = 0; ch < 6; ch++)
			{
				fpos[ch] = (-0.5f * (float)cos((double)t + (double)ch) + 0.5f) * 50.f + 10.f;
			}
			fpos[5] = -fpos[5];

			int size = create_abh_pctl_buf(0x50, 2, fpos, gl_ppp_tx_prestuffing_buffer);
			int stuffed_size = PPP_stuff(gl_ppp_tx_prestuffing_buffer, size, gl_ppp_stuffing_buffer, UNSTUFFING_BUFFER_SIZE);

			LPDWORD num_bytes_written = 0;
			WriteFile(serialport, gl_ppp_stuffing_buffer, stuffed_size, (LPDWORD)(&num_bytes_written), NULL);
		}

		//for (uint64_t st = GetTickCount64(); (GetTickCount64() - st) < 200;)
		{
			LPDWORD num_bytes_read = 0;
			int rc = ReadFile(serialport, gl_ser_readbuf, 512, (LPDWORD)(&num_bytes_read), NULL);	//should be a DOUBLE BUFFER!
			for (int i = 0; i < (int)num_bytes_read; i++)
			{
				uint8_t new_byte = gl_ser_readbuf[i];
				int pld_size = parse_PPP_stream(new_byte, gl_ppp_payload_buffer, PAYLOAD_SIZE, gl_ppp_unstuffing_buffer, UNSTUFFING_BUFFER_SIZE, &gl_ppp_bidx);
				if (pld_size != 0)
				{
					//printf("0x");
					//for (int bi = 0; bi < pld_size; bi++)
					//	printf("%0.2X", gl_ppp_payload_buffer[bi]);
					//printf("\r\n");
					
					//st = 0;
				}
			}
		}

	}
}

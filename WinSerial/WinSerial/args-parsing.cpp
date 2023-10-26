#include "args-parsing.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

cmd_options_t gl_options = {
	460800,	//baud rate
};

/*
* Parse any and all arguments coming in
*/
void parse_args(int argc, char* argv[], cmd_options_t * popts)
{
	if (argc > 1)
	{
		char* tmp;
		for (int i = 1; i < argc; i++)
		{
			if (strcmp("--baudrate", argv[i]) == 0)
			{
				
				if (argc > (i + 1))
				{
					int value = strtol(argv[i + 1], &tmp, 10);
					popts->baud_rate = value;
					printf("Overriding baudrate as %d\r\n", popts->baud_rate);
				}
				else
				{
					printf("invalid baudrate format\r\n");
				}
			}
		}
	}
}

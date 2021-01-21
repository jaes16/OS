#include <system.h>
#include <stdarg.h>

#define ASCII_ALPHA_START 55
#define ASCII_NUM_START 48

unsigned short *text_buf;
unsigned char attribute = 0x7;
unsigned char cursor_x, cursor_y = 0;




//! Displays a formatted string
int DebugPrintf (const char* str, ...) {

	if(!str) return 0;

	va_list args;
	va_start (args, str);
	int i;
	for (i = 0; i < strlen(str); i++) {

		switch (str[i]) {

			case '%':

				switch (str[i+1]) {

					/*** characters ***/
					case 'c':{
						char c;
            c = va_arg (args, char);
						DebugPutc (c);
						i++;		// go to next character
						break;
          }

					/*** address of ***/
					case 's':{
						char *c = (char *) va_arg (args, char);
						char str[64];
            int num_char = strlen(c);
            if (num_char > 63) num_char = 63;
            memset(str, 0, 64);
						memcpy(str, c, strlen(c));
						DebugPuts (str);
						i++;		// go to next character
						break;
          }

					/*** integers ***/
					case 'd':{
            int c = va_arg (args, int);
            char str[32]={0};
            itoa(c, 10, str);
            DebugPuts (str);
            i++;		// go to next character
            break;
          }
					case 'i':{
						int c = va_arg (args, int);
						char str[32]={0};
						itoa(c, 10, str);
						DebugPuts (str);
						i++;		// go to next character
						break;
          }

					/*** display in hex ***/
					case 'X':{
            int c = va_arg (args, int);
            char str[32]={0};
            itoa(c,16,str);
            DebugPuts (str);
            i++;		// go to next character
            break;
          }
					case 'x':{
						int c = va_arg (args, int);
						char str[32]={0};
						itoa(c,16,str);
						DebugPuts (str);
						i++;		// go to next character
						break;
          }


					default:
						va_end (args);
						return 1;
				}

				break;

			default:
				DebugPutc (str[i]);
				break;
		}

	}

	va_end (args);
	return i;
}

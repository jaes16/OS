#include <system.h>

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <libc/string.h>
#include <libc/stdio.h>


//! Displays a formatted string
int printf (const char* str, ...) {

	if(!str) return 0;

	va_list args;
	va_start (args, str);

  int ret_val = 0;
	for (int i = 0; i < strlen(str); i++) {

		switch (str[i]) {

			case '%':

				switch (str[i+1]) {

					/*** characters ***/
					case 'c':{
						char c = va_arg (args, int);
						putc (c);
            ret_val++;
						break;
          }

					/*** address of ***/
					case 's':{
						char *c = (char *) va_arg (args, int);
						char str[64];
            int num_char = strlen(c);
            if (num_char > 63) num_char = 63;
            memset(str, 0, 64);
						memcpy(str, c, strlen(c));
						puts (str);
            ret_val += num_char;
						break;
          }

					/*** integers ***/
					case 'd':{
            int c = va_arg (args, int);
            char str[32]={0};
            itoa(c, 10, str);
            puts (str);
            ret_val += strlen(str);
            break;
          }
					case 'i':{
						int c = va_arg (args, int);
						char str[32]={0};
						itoa(c, 10, str);
						puts (str);
            ret_val += strlen(str);
						break;
          }

					/*** display in hex ***/
					case 'X':{
            int c = va_arg (args, int);
            char str[32]={0};
            itoa(c,16,str);
            puts (str);
            ret_val += strlen(str);
            break;
          }
					case 'x':{
						int c = va_arg (args, int);
						char str[32]={0};
						itoa(c,16,str);
						puts (str);
            ret_val += strlen(str);
						break;
          }


					default:
						va_end (args);
						return 1;
				}

        i++;
				break;

			default:
				putc (str[i]);
        ret_val++;
				break;
		}

	}

	va_end (args);
	return ret_val;
}

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
	for (int i = 0; i < (int) strlen(str); i++) {

		switch (str[i]) {

			case '%':

				switch (str[i+1]) {

					case 'c':{
						char c = va_arg (args, int);
						putc (c);
            ret_val++;
						break;
          }


					case 's':{
						char *c = (char *) va_arg (args, int);
						char s[64];
            int num_char = strnlen(c,64);
            if (num_char > 63) num_char = 63;
            memset(s, 0, 64);
						memcpy(s, c, num_char);
						puts (s);
            ret_val += num_char;
						break;
          }


					case 'd':{
            int c = va_arg (args, int);
            char s[32]={0};
            itoa(c, 10, s);
            puts (s);
            ret_val += strlen(s);
            break;
          }
					case 'i':{
						int c = va_arg (args, int);
						char s[32]={0};
						itoa(c, 10, s);
						puts (s);
            ret_val += strlen(s);
						break;
          }
					case 'X':{
            int c = va_arg (args, int);
            char s[32]={0};
            itoa(c,16,s);
            puts(s);
            ret_val += strlen(s);
            break;
          }
					case 'x':{
						int c = va_arg (args, int);
						char s[32]={0};
						itoa(c,16,s);
						puts(s);
            ret_val += strlen(s);
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

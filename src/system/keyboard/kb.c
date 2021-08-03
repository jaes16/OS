#include <system.h>

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <libc/string.h>
#include <libc/stdio.h>

#define KB_ESCAPE 0x1
#define KB_CONTROL 0x1d
#define KB_LEFT_SHIFT 0x2a
#define KB_RIGHT_SHIFT 0x36
#define KB_ALT 0x38


#define KB_CONTROL_REL 0x9d
#define KB_LEFT_SHIFT_REL 0xaa
#define KB_RIGHT_SHIFT_REL 0xb6
#define KB_ALT_REL 0xb8

unsigned char kbdus[128] =
{
    0,  KB_ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    KB_CONTROL,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   KB_LEFT_SHIFT,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/', KB_RIGHT_SHIFT,			/* Right shift */
  '*',
    KB_ALT,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0/*x80*/,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0/*x81*/,	/* Left Arrow */
    0,
    0/*x82*/,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0/*x83*/,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

unsigned char kbdus_shift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    KB_CONTROL,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
 '\"', '~',   KB_LEFT_SHIFT,		/* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?', KB_RIGHT_SHIFT,			/* Right shift */
  '*',
    KB_ALT,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0/*x80*/,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0/*x81*/,	/* Left Arrow */
    0,
    0/*x82*/,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0/*x83*/,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};


volatile int kb_receiving_input = 1;
char kb_command[80];
volatile char cur_char;

char shift_r_press = 0;
char shift_l_press = 0;
char ctrl_press = 0;
char alt_press = 0;

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{
	(void)(r);

  if(kb_receiving_input == 0) return;

	// if we have an input to read
  if(inportb(0x64) & 1){
    unsigned char scancode;

    // read keyboard input
    scancode = inportb(0x60);
    if ((scancode & 0x80) == 0){ // pressed a key
			if (scancode == KB_CONTROL) ctrl_press = 1;
			else if (scancode == KB_LEFT_SHIFT) shift_l_press = 1;
			else if (scancode == KB_RIGHT_SHIFT) shift_r_press = 1;
			else if (scancode == KB_ALT) alt_press = 1;
			else{
				switch(kbdus[scancode]){
					case('\b'):{
	          //VGA_backspace();
	          cur_char = '\b';
	          break;
	        }
	        case('\n'):{
	          //putc('\n');
	          cur_char = '\n';
	          break;
	        }
					default:{
	          if((VGA_crsr_pos() % 80) < 79){
							if (shift_l_press == 1 || shift_r_press == 1) cur_char = kbdus_shift[scancode];
							else cur_char = kbdus[scancode];
						}
	          break;
	        }
				}
			}
    }
		else { // released key
			switch(scancode){
				case (KB_CONTROL_REL):{
					ctrl_press = 0;
					break;
				}
				case (KB_LEFT_SHIFT_REL):{
					shift_l_press = 0;
					break;
				}
				case (KB_RIGHT_SHIFT_REL):{
					shift_r_press = 0;
					break;
				}
				case (KB_ALT_REL):{
					alt_press = 0;
					break;
				}
      }
		}
  }

  kb_receiving_input = 0;
  return;
}

char getChar(void)
{
  kb_receiving_input = 1;
  while(kb_receiving_input == 1);
  char ret_val = cur_char;
  cur_char = 0;
  return ret_val;
}


void keyboard_install(void)
{
  irq_install_handler(1, keyboard_handler);
}

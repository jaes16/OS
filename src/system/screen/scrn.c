#include <system.h>

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <libc/string.h>
#include <libc/stdio.h>

#define VGA_BUF_LOC     0xB8000
#define VGA_SIZE        2000
#define VGA_Y_SIZE      25
#define VGA_X_SIZE      80
#define VGA_ENTRY_SIZE  2

#define VGA_ATTRIBUTE   0xf
#define VGA_BLANK       ((VGA_ATTRIBUTE << 8) | 0)

unsigned short *text_buf;
unsigned short temp_buf[VGA_Y_SIZE*VGA_X_SIZE];
unsigned char cursor_x = 0, cursor_y = 0;

/* Scrolls the screen one line*/
void scroll(void)
{
  // copy last 24 lines up
  memcpyw(text_buf, &text_buf[VGA_X_SIZE], (VGA_Y_SIZE - 1) * VGA_X_SIZE);

  // erase last line
  memsetw(&text_buf[(VGA_Y_SIZE - 1) * VGA_X_SIZE], VGA_BLANK, VGA_X_SIZE);

  // reset cursor
  cursor_y = VGA_Y_SIZE - 1;
}

int VGA_crsr_pos(void)
{
  return (cursor_y * VGA_X_SIZE) + cursor_x;
}


// clears screen and moves cursor
void cls()
{
  memsetw(text_buf, VGA_BLANK, VGA_X_SIZE * VGA_Y_SIZE);
}


// updates cursor
void move_crsr(void)
{
  /*
  // if cursor is at an invalid (value 0) entry, move to leftmost invalid entry
  while((text_buf[VGA_crsr_pos()] == VGA_BLANK) && (text_buf[VGA_crsr_pos()-1] == VGA_BLANK)){
    if(cursor_x > 0) cursor_x--;
    else if(cursor_y > 0){
      cursor_y--;
      cursor_x = VGA_X_SIZE-1;
    }
  }
  */

  // index of cursor
  unsigned crsr_position = VGA_crsr_pos();

  /* This sends a command to indicies 14 and 15 in the
  *  CRT Control Register of the VGA controller. These
  *  are the high and low bytes of the index that show
  *  where the hardware cursor is to be 'blinking'. */
  outportb(0x3D4, 14);
  outportb(0x3D5, crsr_position >> 8);
  outportb(0x3D4, 15);
  outportb(0x3D5, crsr_position);
}



void VGA_backspace(void) // explicitly for terminal commands
{
  if(cursor_x > 1){
    cursor_x--;
    text_buf[VGA_crsr_pos()] = VGA_BLANK;
    move_crsr();
  }
}



void itoa(unsigned num,unsigned base,char* buf) {

  if (num == 0 || base > 16) {
    buf[0] = '0';
    buf[1] = 0;
    return;
  }

  char temp[32];
  char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  int pos = 0;

  while (num != 0) {
    temp[pos] = bchars[num % base];
    pos++;
    num /= base;
  }

  if(base == 16){
    temp[pos++] = 'x';
    temp[pos++] = '0';
  }

  int top = pos--;
  int opos = 0;

  for (opos = 0; opos<top; pos--,opos++) {
    buf[opos] = temp[pos];
  }
  buf[opos] = 0;
}


// Displays a character
void putc(char c) {

  // backspace character
  if (c == 0x08 && cursor_x){
    cursor_x--;
    text_buf[VGA_crsr_pos()] = VGA_BLANK;
  }

  // tab character
  else if (c == 0x09) cursor_x = (cursor_x+8) & ~(8-1);

  // carriage return
  else if (c == '\r') cursor_x = 0;

  // new line
	else if (c == '\n') {
    cursor_x = 0;
    cursor_y++;
	}

  // printable characters
  else if(c >= ' ') {

    // display character on screen
    text_buf[VGA_crsr_pos()] = c | (VGA_ATTRIBUTE << 8);
    cursor_x++;
  }

  // if we are at edge of row, go to new line
  if (cursor_x >= VGA_X_SIZE) {
    cursor_x = 0;
    cursor_y++;
  }

  if (cursor_y >= VGA_Y_SIZE) scroll();

  // update hardware cursor
	move_crsr();
}


void puts(char* str) {
  int len = strlen(str);
  for (int i = 0 ; i < len; i++) putc(str[i]);
}

/* Sets our text-mode VGA pointer, then clears the screen for us */
void init_video(void)
{
  cursor_x = 0;
  cursor_y = 0;
  text_buf = (unsigned short *) VGA_BUF_LOC;
  cls();
}

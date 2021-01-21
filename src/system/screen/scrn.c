#include <system.h>
#include <stdarg.h>

#define VGA_BUF_LOC 0xB8000
#define VGA_SIZE 2000
#define VGA_Y_SIZE 25
#define VGA_X_SIZE 80
#define VGA_ENTRY_SIZE 2 // 2 bytes

#define TAB_SIZE

unsigned short *text_buf;
unsigned short temp_buf[VGA_Y_SIZE*VGA_X_SIZE];
unsigned char attribute = 0xf;
unsigned char cursor_x = 0, cursor_y = 0;
unsigned short zero;
unsigned short nl;

/* Scrolls the screen one line*/
void scroll(void)
{
  // copy last 24 lines up
  memcpyw(text_buf, &text_buf[VGA_X_SIZE], (VGA_Y_SIZE - 1) * VGA_X_SIZE);

  // erase last line
  memsetw(&text_buf[(VGA_Y_SIZE - 1) * VGA_X_SIZE], zero, VGA_X_SIZE);

  // reset cursor
  cursor_y = VGA_Y_SIZE - 1;
}

int crsr_pos(void)
{
  return (cursor_y * VGA_X_SIZE) + cursor_x;
}


// clears screen and moves cursor
void cls()
{
  memsetw(text_buf, zero, VGA_X_SIZE * VGA_Y_SIZE);
}


// updates cursor
void move_crsr(void)
{
  /*
  // if cursor is at an invalid (value 0) entry, move to leftmost invalid entry
  while((text_buf[crsr_pos()] == zero) && (text_buf[crsr_pos()-1] == zero)){
    if(cursor_x > 0) cursor_x--;
    else if(cursor_y > 0){
      cursor_y--;
      cursor_x = VGA_X_SIZE-1;
    }
  }
  */

  // index of cursor
  unsigned crsr_position = crsr_pos();

  /* This sends a command to indicies 14 and 15 in the
  *  CRT Control Register of the VGA controller. These
  *  are the high and low bytes of the index that show
  *  where the hardware cursor is to be 'blinking'. */
  outportb(0x3D4, 14);
  outportb(0x3D5, crsr_position >> 8);
  outportb(0x3D4, 15);
  outportb(0x3D5, crsr_position);
}

void itoa(unsigned i,unsigned base,char* buf) {
  char tbuf[32];
  char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

  int pos = 0;
  int opos = 0;
  int top = 0;

  if (i == 0 || base > 16) {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }

  while (i != 0) {
    tbuf[pos] = bchars[i % base];
    pos++;
    i /= base;
  }
  top=pos--;
  for (opos=0; opos<top; pos--,opos++) {
    buf[opos] = tbuf[pos];
  }
  buf[opos] = 0;
}


// Displays a character
void putc(char c) {

  // backspace character
  if (c == 0x08 && cursor_x) cursor_x--;

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
    text_buf[(cursor_y * VGA_X_SIZE) + cursor_x] = c | (attribute << 8);
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
  for (int i = 0 ; i < strlen(str); i++) putc(str[i]);
}



/* Sets the forecolor and backcolor that we will use */
void settextcolor(unsigned char forecolor, unsigned char backcolor)
{
  /* Top 4 bits are the background, bottom 4 bits
  *  are the foreground color */
  attribute = (backcolor << 4) | (forecolor & 0x0F);
}

/* Sets our text-mode VGA pointer, then clears the screen for us */
void init_video(void)
{
  cursor_x = 0;
  cursor_y = 0;
  attribute = 0xf;
  nl = '\n' & (0x0f);
  nl = nl | (attribute << 8);
  zero = attribute << 8;
  zero = zero | ' ';
  text_buf = (unsigned short *)0xB8000;
  cls();
}


#include <system.h>
#include <libc/stdio.h>

#define		I86_PIT_REG_COUNTER0		0x40
#define		I86_PIT_REG_COUNTER1		0x41
#define		I86_PIT_REG_COUNTER2		0x42
#define		I86_PIT_REG_COMMAND			0x43

void timer_phase(int hz)
{
  int divisor = 1193180 / hz;       				// calculate our divisor
  outportb(I86_PIT_REG_COMMAND, 0x36);      // set our command byte 0x36
  outportb(I86_PIT_REG_COUNTER0, divisor & 0xFF);   // set low byte of divisor
  outportb(I86_PIT_REG_COUNTER0, divisor >> 8);     // set high byte of divisor
}

int timer_ticks = 0;

// from irq interrupt
void timer_handler(struct regs *r)
{
	(void)(r);

  timer_ticks++;

  return;
}

// loop till we have enough interrupts
void timer_wait(int ticks)
{
  unsigned long eticks;

  eticks = timer_ticks + ticks;
  while(((unsigned long) timer_ticks) < eticks);
}

void timer_install(void)
{

  // install irq handler for timer
  irq_install_handler(0, timer_handler);
}

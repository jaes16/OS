#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>




/* MAIN.C */
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);
extern unsigned short inportw (unsigned short _port);
extern void outportw (unsigned short _port, unsigned short _data);

/* SCRN.C */
extern void scroll(void);
extern void itoa(unsigned i,unsigned base,char* buf);
extern int VGA_crsr_pos(void);
extern void VGA_backspace(void);
extern void putc(char c);
extern void puts(char *str);
extern int DebugPrintf (const char* str, ...);
extern void init_video(void);

/* GDT.C */
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install(void);

/* IDT.C */
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install(void);

/* ISR.C */
/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */
};
extern void isrs_install(void);
extern void fault_handler(struct regs *r);

/* IRQ.C */
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void irq_remap(void);
extern void irq_install(void);
extern void irq_handler(struct regs *r);

extern void interrupt_done(unsigned int n);

/* TIMER.C */
extern int timer_ticks;
extern void timer_phase(int hz);
extern void timer_wait(int ticks);
extern void timer_install(void);

/* KB.C */
extern void keyboard_handler(struct regs *r);
extern void keyboard_install(void);
extern char getChar(void);

#endif

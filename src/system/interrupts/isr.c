#include <system.h>

// Interrupt service routines

/* These are function prototypes for all of the exception
*  handlers: The first 32 entries in the IDT are reserved
*  by Intel, and are designed to service exceptions! */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

/* This is a very repetitive function... it's not hard, it's
*  just annoying. As you can see, we set the first 32 entries
*  in the IDT to the first 32 ISRs. We can't use a for loop
*  for this, because there is no way to get the function names
*  that correspond to that given entry. We set the access
*  flags to 0x8E. This means that the entry is present, is
*  running in ring 0 (kernel level), and has the lower 5 bits
*  set to the required '14', which is represented by 'E' in
*  hex. */
void isrs_install(void)
{
  unsigned long address = (unsigned long)&isr0;
  idt_set_gate(0, address, 0x08, 0x8E);
  address = (unsigned long)&isr1;
  idt_set_gate(1, address, 0x08, 0x8E);
  address = (unsigned long)&isr2;
  idt_set_gate(2, address, 0x08, 0x8E);
  address = (unsigned long)&isr3;
  idt_set_gate(3, address, 0x08, 0x8E);
  address = (unsigned long)&isr4;
  idt_set_gate(4, address, 0x08, 0x8E);
  address = (unsigned long)&isr5;
  idt_set_gate(5, address, 0x08, 0x8E);
  address = (unsigned long)&isr6;
  idt_set_gate(6, address, 0x08, 0x8E);
  address = (unsigned long)&isr7;
  idt_set_gate(7, address, 0x08, 0x8E);
  address = (unsigned long)&isr8;
  idt_set_gate(8, address, 0x08, 0x8E);
  address = (unsigned long)&isr9;
  idt_set_gate(9, address, 0x08, 0x8E);
  address = (unsigned long)&isr10;
  idt_set_gate(10, address, 0x08, 0x8E);
  address = (unsigned long)&isr11;
  idt_set_gate(11, address, 0x08, 0x8E);
  address = (unsigned long)&isr12;
  idt_set_gate(12, address, 0x08, 0x8E);
  address = (unsigned long)&isr13;
  idt_set_gate(13, address, 0x08, 0x8E);
  address = (unsigned long)&isr14;
  idt_set_gate(14, address, 0x08, 0x8E);
  address = (unsigned long)&isr15;
  idt_set_gate(15, address, 0x08, 0x8E);
  address = (unsigned long)&isr16;
  idt_set_gate(16, address, 0x08, 0x8E);
  address = (unsigned long)&isr17;
  idt_set_gate(17, address, 0x08, 0x8E);
  address = (unsigned long)&isr18;
  idt_set_gate(18, address, 0x08, 0x8E);
  address = (unsigned long)&isr19;
  idt_set_gate(19, address, 0x08, 0x8E);
  address = (unsigned long)&isr20;
  idt_set_gate(20, address, 0x08, 0x8E);
  address = (unsigned long)&isr21;
  idt_set_gate(21, address, 0x08, 0x8E);
  address = (unsigned long)&isr22;
  idt_set_gate(22, address, 0x08, 0x8E);
  address = (unsigned long)&isr23;
  idt_set_gate(23, address, 0x08, 0x8E);
  address = (unsigned long)&isr24;
  idt_set_gate(24, address, 0x08, 0x8E);
  address = (unsigned long)&isr25;
  idt_set_gate(25, address, 0x08, 0x8E);
  address = (unsigned long)&isr26;
  idt_set_gate(26, address, 0x08, 0x8E);
  address = (unsigned long)&isr27;
  idt_set_gate(27, address, 0x08, 0x8E);
  address = (unsigned long)&isr28;
  idt_set_gate(28, address, 0x08, 0x8E);
  address = (unsigned long)&isr29;
  idt_set_gate(29, address, 0x08, 0x8E);
  address = (unsigned long)&isr30;
  idt_set_gate(30, address, 0x08, 0x8E);
  address = (unsigned long)&isr31;
  idt_set_gate(31, address, 0x08, 0x8E);
}

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
unsigned char *exception_messages[] =
{
  (unsigned char *)"Division By Zero",
  (unsigned char *)"Debug",
  (unsigned char *)"Non Maskable Interrupt",
  (unsigned char *)"Breakpoint",
  (unsigned char *)"Into Detected Overflow",
  (unsigned char *)"Out of Bounds",
  (unsigned char *)"Invalid Opcode",
  (unsigned char *)"No Coprocessor",
  (unsigned char *)"Double Fault",
  (unsigned char *)"Coprocessor Segment Overrun",
  (unsigned char *)"Bad TSS",
  (unsigned char *)"Segment Not Present",
  (unsigned char *)"Stack Fault",
  (unsigned char *)"General Protection Fault",
  (unsigned char *)"Page Fault",
  (unsigned char *)"Unknown Interrupt",
  (unsigned char *)"Coprocessor Fault",
  (unsigned char *)"Alignment Check",
  (unsigned char *)"Machine Check",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved",
  (unsigned char *)"Reserved"
};

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void fault_handler(struct regs *r)
{
  /* Is this a fault whose number is from 0 to 31? */
  if (r->int_no < 32)
  {
    /* Display the description for the Exception that occurred.
    *  In this tutorial, we will simply halt the system using an
    *  infinite loop */
    puts((char *) exception_messages[r->int_no]);
    puts(" Exception. System Halted!\n");
    for (;;);
  }
}

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

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void fault_handler(struct regs *r)
{

  unsigned char except_0[30] = "Division By Zero";
  unsigned char except_1[30] = "Debug";
  unsigned char except_2[30] = "Non Maskable Interrupt";
  unsigned char except_3[30] = "Breakpoint";
  unsigned char except_4[30] = "Into Detected Overflow";
  unsigned char except_5[30] = "Out of Bounds";
  unsigned char except_6[30] = "Invalid Opcode";
  unsigned char except_7[30] = "No Coprocessor";
  unsigned char except_8[30] = "Double Fault";
  unsigned char except_9[30] = "Coprocessor Segment Overrun";
  unsigned char except_10[30] = "Bad TSS";
  unsigned char except_11[30] = "Segment Not Present";
  unsigned char except_12[30] = "Stack Fault";
  unsigned char except_13[30] = "General Protection Fault";
  unsigned char except_14[30] = "Page Fault";
  unsigned char except_15[30] = "Unknown Interrupt";
  unsigned char except_16[30] = "Coprocessor Fault";
  unsigned char except_17[30] = "Alignment Check";
  unsigned char except_18[30] = "Machine Check";
  unsigned char except_19[30] = "Reserved";
  unsigned char except_20[30] = "Reserved";
  unsigned char except_21[30] = "Reserved";
  unsigned char except_22[30] = "Reserved";
  unsigned char except_23[30] = "Reserved";
  unsigned char except_24[30] = "Reserved";
  unsigned char except_25[30] = "Reserved";
  unsigned char except_26[30] = "Reserved";
  unsigned char except_27[30] = "Reserved";
  unsigned char except_28[30] = "Reserved";
  unsigned char except_29[30] = "Reserved";
  unsigned char except_30[30] = "Reserved";
  unsigned char except_31[30] = "Reserved";

  char standard_message[30] = " Exception. System Halted!\n";


  unsigned char *exception_messages[] =
  {
    except_0, except_1, except_2, except_3,
    except_4, except_5, except_6, except_7,
    except_8, except_9, except_10, except_11,
    except_12, except_13, except_14, except_15,
    except_16, except_17, except_18, except_19,
    except_20, except_21, except_22, except_23,
    except_24, except_25, except_26, except_27,
    except_28, except_29, except_30, except_31
  };

  


  /* Is this a fault whose number is from 0 to 31? */
  if (r->int_no < 32)
  {
    /* Display the description for the Exception that occurred.
    *  In this tutorial, we will simply halt the system using an
    *  infinite loop */
    puts((char *) exception_messages[r->int_no]);
    puts(standard_message);
    for (;;);
  }
}

// Adapted from Bran's Kernel Development Tutorial

#include <system.h>
#include <libc/string.h>

#define IDT_ENTRY_COUNT   256
#define IDT_ENTRY_SIZE    8
#define IDT_SIZE          IDT_ENTRY_COUNT * IDT_ENTRY_SIZE


// defines an idt entry, packed so gcc doesn't mess with it
struct idt_entry
{
  unsigned short base_lo;
  unsigned short sel;        /* Our kernel segment goes here! */
  unsigned char always0;     /* This will ALWAYS be set to 0! */
  unsigned char flags;       /* Set using the above table! */
  unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
  unsigned short limit;
  unsigned int base;
} __attribute__((packed));

// much of the entries of the idt are there to cause a unhandled interrupt fault
struct idt_entry idt[IDT_ENTRY_COUNT];
// pointer to idt that will be used as an external variable in 'idt.s'
struct idt_ptr idtp;

// function in assembly file 'idt.s', uses instruction lidt to load idt
extern void idt_load();

// setting idt entry
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
  // set idt entry
  idt[num].base_hi = (base>>16) & 0xffff;
  idt[num].base_lo = (base & 0xffff);
  idt[num].sel = sel;
  idt[num].always0 = 0;
  idt[num].flags = flags;
}

// installs the idt
void idt_install(void)
{

  memset(&idt, 0, IDT_SIZE);

    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = IDT_SIZE - 1;
    idtp.base = (unsigned int) &idt;


    idt_load();
}

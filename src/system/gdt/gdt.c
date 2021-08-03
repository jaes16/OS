#include <system.h>


// ACCESS BITS
// access bit: set to 0, cpu will set to 1 when accessed
#define GDT_ACCESS_AC   0x01
// read/write: read for code segments, write for data segments
#define GDT_ACCESS_RW   0x02
// direction/conforming bit: for data segments, 0 for upwards growth, 1 for downwards growth
//                           for code segments, 0 for execution only with this privilege level, 1 for execution for any privilege level lower (higher ring level) than this
#define GDT_ACCESS_DC   0x04
// executable bit: 0 for data, 1 for code
#define GDT_ACCESS_EX   0x08
// descriptor type: 0 for system segments, 1 for code or data segments
#define GDT_ACCESS_S    0x10
// privilege bit: ring level: 0 = kernel, 3 = user
#define GDT_ACCESS_PRV  0x60
// present bit: 1 for all present segments
#define GDT_ACCESS_PR   0x80


// FLAG BITS
// size bit: 0 for 16 bit protected mode, 1 for 32 bit protected mode
#define GDT_FLAG_SIZE   0x4
// granularity bit: 0 for 1 byte granularity, 1 for 4 kB block granularity
#define GDT_FLAG_GRAN   0x8

// prevent compiler optimization by packing
struct gdt_entry
{
  unsigned short limit_low;
  unsigned short base_low;
  unsigned char base_middle;
  unsigned char access;
  unsigned char limit_high_n_flags;
  unsigned char base_high;
} __attribute__((packed));

// gdtable pointer
struct gdt_ptr
{
  unsigned short limit;
  unsigned int base;
} __attribute__((packed));

// entries for empty, code, data
struct gdt_entry gdt[3];
struct gdt_ptr gp;

// calls load gdt in gdt.s
extern void gdt_flush();

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char flags)
{
	// gdt region address
  gdt[num].base_low = (base & 0xFFFF);
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;

	// gdt region limit
  gdt[num].limit_low = (limit & 0xFFFF);
  gdt[num].limit_high_n_flags = ((limit >> 16) & 0x0F);

	// gdt region flags
  gdt[num].limit_high_n_flags |= ((flags << 4) & 0xF0);
  gdt[num].access = access;
}

// sets up gdt
void gdt_install(void)
{
	// set up pointer to gdt
  gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
  gp.base = (unsigned int) &gdt;

  // gdt has to have one null descriptor.
  gdt_set_gate(0, 0, 0, 0, 0);

  // code segment entry: access: readable, executable (code segment), not a system segment, and present
  //                     flags: 32 bit protected mode, 4kB block granularity
  unsigned char gdt_access = 0 | GDT_ACCESS_RW | GDT_ACCESS_EX | GDT_ACCESS_S | GDT_ACCESS_PR;
  unsigned char gdt_flags = 0 | GDT_FLAG_SIZE | GDT_FLAG_GRAN;
  gdt_set_gate(1, 0, 0xFFFFFFFF, gdt_access, gdt_flags);

  // data segment entry: same as above, but executable bit is =0 in access byte now
  gdt_access = 0 | GDT_ACCESS_RW | GDT_ACCESS_S | GDT_ACCESS_PR;
  gdt_set_gate(2, 0, 0xFFFFFFFF, gdt_access, gdt_flags);

  gdt_flush();
}

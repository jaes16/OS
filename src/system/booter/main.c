#include <system.h>
#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>
#include <mmngr_virtual.h>


// for reading in the memory regions given to us by GRUB
typedef struct memory_region {
  unsigned long base_addr;
  unsigned long length;
  unsigned int type;
  unsigned int reserved;
} memory_region;

// copies count bytes of src to dest, but if range src:src+count is within dest:dest+count, src may change before written
void *memcpy(void* dest, const void *src, size_t count)
{
  for(size_t i = 0; i < count; i++){
    ((char *)dest)[i] = ((char *)src)[i];
  }
  return dest;
}

// sets count bytes of dest to the value in val
void *memset(void* dest, char val, size_t count)
{
  for(size_t i = 0; i < count; i++){
    ((char *)dest)[i] = val;
  }
  return dest;
}

// copies count bytes of src to dest, but if range src:src+count is within dest:dest+count, src may change before written
void *memcpyw(void* dest, const void *src, size_t count)
{
  unsigned short temp[count];
  for(size_t i = 0; i < count; i++){
    temp[i] = ((unsigned short *)src)[i];
  }
  for(size_t i = 0; i < count; i++){
    ((unsigned short *)dest)[i] = temp[i];
  }
  return dest;
}

// sets count shortbytes of dest to the value in val
unsigned short *memsetw(void *dest, unsigned short val, size_t count)
{
  for(size_t i = 0; i < count; i++){
    ((unsigned short *)dest)[i] = val;
  }
  return dest;
}

int strlen(const char *str)
{
  int i = 0;
  while(str[i] != 0) i++;
  return i;
}

// inline assembly for reading from port
unsigned char inportb (unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

// inline assembly for writing out to port
void outportb (unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}



/*/////////////////////////////////////////////////////////////////////////////
/////////////////// MAIN FUNCTION: STARTS HERE ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////*/
void _main(multiboot_info_t* mbd, unsigned long kernel_end_addr, unsigned long kernel_start, unsigned int magic)
{
  // if magic number for multiboot is incorrect, halt. something don goofed
  if(magic != 0x2BADB002){
    // init_video();
    // puts("Incorrect magic number.");
    __asm__ __volatile__ ("hlt");
    for(;;);
  }

  // initialize VGA; gdt; idt, irq, isrs; timer; keyboard input
  gdt_install();
  idt_install();
  irq_install();
  isrs_install();
  timer_install();
  keyboard_install();


  // then start interrupts so those installs above have an effect
  __asm__ __volatile__ ("sti");


  // start reading multiboot info to get memory size and locations
  struct multiboot_mmap_entry *m_region = (struct multiboot_mmap_entry *) mbd->mmap_addr;
  // mem_lower is return in kilobytes and mem_upper is returned in a multiple of 64 kilobytes
  int phys_mem_size = mbd->mem_lower + (mbd->mem_upper * 64); // leave it in kilobytes because we want ints
  pmmngr_init(phys_mem_size, kernel_end_addr);

  //pmmngr_init_region(0xB8000, 4000); // VGA location
  //char *b = pmmngr_alloc_block();

  // free those regions that grub said are free
  for(int i = 0; m_region[i].size > 0; i++){
    if(m_region[i].type == 1){
      pmmngr_init_region(m_region[i].addr, m_region[i].len);
    }
  }

  // set as used: memorymap used for physical_memory manager; kernel space;
  pmmngr_deinit_region(kernel_start, kernel_end_addr-kernel_start); // deep space, outer space, inner space, kernel space. I'm no astrophysicist
  pmmngr_deinit_region(kernel_end_addr, (phys_mem_size/PMMNGR_BLOCK_SIZE)/PMMNGR_BLOCKS_PER_BYTE); // space for physical memory manager

  vmmngr_initialize();

  init_video();

  char num[32];
  num[0] = 'H';
  num[1] = 'e';
  num[2] = 'l';
  num[3] = 'l';
  num[4] = 'o';
  num[5] = ' ';
  num[6] = 'W';
  num[7] = 'o';
  num[8] = 'r';
  num[9] = 'l';
  num[10] = 'd';
  num[11] = '!';
  num[12] = 0;
  puts(num);
  //puts("Hello World!");

  /*
  char num[32];
  puts("PMM initialized with ");
  itoa(phys_mem_size, 10, num);
  puts(num);
  puts("KB of physical memory.\n\nPhysical Memory Map:\n");

  int i = 0;
  int phys_mem_size_2 = 0;
  while(m_region[i].size > 0){
    memset(num, 0, 32);
    puts((char *) "Region ");
    putc((char) (i+48));
    puts((char *) ": start: 0x");
    itoa(m_region[i].addr, 16, num);
    puts(num);
    puts((char *) " length (bytes): 0x");
    memset(num, 0, 32);
    itoa(m_region[i].len, 16, num);
    puts(num);
    puts((char *) " type: ");
    puts((char *) (m_region[i].type + 48));
    switch(m_region[i].type){
      case(1):
        puts((char *) " (Available)\n");
        phys_mem_size_2 += m_region[i].len;
        break;
      case(2):
        puts((char *) " (Reserved)\n");
        break;
      case(3):
        puts((char *) " (ACPI Reclaimable)\n");
        break;
      case(4):
        puts((char *) " (NVS)\n");
        break;
      case(5):
        puts((char *) " (BADRAM)\n");
        break;
    }
    i++;
  }

  puts("\nCalculated memory size: ");
  memset(num, 0, 32);
  itoa(phys_mem_size_2, 16, num);
  puts(num);


  puts("\nMemory size: ");
  memset(num, 0, 32);
  itoa(pmmngr_get_memory_size(), 16, num);
  puts(num);

  puts("\nBlock count: ");
  memset(num, 0, 32);
  itoa(pmmngr_get_block_count(), 16, num);
  puts(num);

  puts("\nFree block count: ");
  memset(num, 0, 32);
  itoa(pmmngr_get_free_block_count(), 16, num);
  puts(num);

  puts("\nFreeing block at: 0xFE00000");
  pmmngr_free_block((void *) 0xfe00000);

  puts("\nFree block count: ");
  memset(num, 0, 32);
  itoa(pmmngr_get_free_block_count(), 16, num);
  puts(num);

  puts("\nAllocating block at: ");
  memset(num, 0, 32);
  itoa((int) pmmngr_alloc_block(), 16, num);
  puts(num);

  puts("\nFree Block count: ");
  memset(num, 0, 32);
  itoa(pmmngr_get_free_block_count(), 16, num);
  puts(num);
  */


  for (;;);
}

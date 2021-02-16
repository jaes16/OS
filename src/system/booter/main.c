#include <system.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <multiboot.h>
#include <mmngr_virtual.h>
#include <ata.h>


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

// inline assembly for reading from port
unsigned short inportw (unsigned short _port)
{
    unsigned short rv;
    __asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

// inline assembly for writing out to port
void outportw (unsigned short _port, unsigned short _data)
{
    __asm__ __volatile__ ("outw %1, %0" : : "dN" (_port), "a" (_data));
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
  init_video();

  puts("Before c paging.\n");
  // then start interrupts so those installs above have an effect

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

  char sector_0[512];
  memsetw(sector_0, 0, 256);

  puts("Identifying drives : master\n\n");
  ATA_PIO_identify(0);
  puts("Identifying drives : slave\n\n");
  ATA_PIO_identify(0);

  puts("Preparing to read and write sectors\n\n");

  ATA_PIO_read_sectors(sector_0, 0, 1);
  puts("Read sector:\n");
  for(int i = 0; i < 512; i++){
    if (sector_0[i] != 0) putc(sector_0[i]);
  }
  puts("\n\n");

  memset(sector_0, 65, 512);
  puts("Writing out this:");
  //puts(sector_0);
  puts("\n\n");

  ATA_PIO_write_sectors(sector_0, 0, 1);
  puts("Wrote sector\n\n");


  puts("Preparing to read sector\n");
  ATA_PIO_read_sectors(sector_0, 0, 1);
  puts("Read in: \n");
  for(int i = 0; i < 512; i++) {
    if (sector_0[i] != 0) putc(sector_0[i]);
  }
  putc('\n');

  memset(sector_0, 0, 512);
  ATA_PIO_write_sectors(sector_0, 0, 1);

  iconic_text(sector_0, 512);
  ATA_PIO_write_sectors(sector_0, 0, 1);
  puts("Wrote sector\n\n");

  memset(sector_0, '!', 512);
  puts(sector_0);

  puts("Preparing to read sector\n");
  ATA_PIO_read_sectors(sector_0, 0, 1);
  puts(sector_0);

__asm__ __volatile__ ("sti");

  for (;;);
}

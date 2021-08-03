
#include <system.h>

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <multiboot.h>

#include <mmngr_virtual.h>

#include <ata.h>
#include <pci.h>
#include <fat_fs.h>
#include <terminal.h>


uint32_t pmap_address;
uint32_t pmap_size;
uint32_t fat_address;
uint32_t fat_size;

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

// inline assembly for reading from port (in sizes of 2 bytes)
unsigned short inportw (unsigned short _port)
{
  unsigned short rv;
  __asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (_port));
  return rv;
}

// inline assembly for writing out to port (in sizes of 2 bytes)
void outportw (unsigned short _port, unsigned short _data)
{
  __asm__ __volatile__ ("outw %1, %0" : : "dN" (_port), "a" (_data));
}

unsigned int inportd (unsigned short _port)
{
  unsigned int rv;
  __asm__ __volatile__ ("inl %1, %0" : "=a" (rv) : "dN" (_port));
  return rv;
}

void outportd (unsigned short _port, unsigned int _data)
{
  __asm__ __volatile__ ("outl %1, %0" : : "dN" (_port), "a" (_data));
}



// get input till \n
int start_input(void)
{
	// buffer for current line in terminal
  char buf[TERM_MAX_COM_LEN];

  memset(buf, 0, TERM_MAX_COM_LEN);
  int index = 0;

  int ret_val = 1;

  printf("\nroot %s >", fat_cur_path);

  int looper = 1;
  while(looper == 1){

    char c = getChar();

    if(c == '\b'){
			// backspace unless at beginning
      if(index > 0){
				VGA_backspace();
        index--;
        buf[index] = 0;
      }
    }
    else if(c == '\n'){
			putc('\n');
      char temp[TERM_MAX_COM_LEN];
      memset(temp, 0, TERM_MAX_COM_LEN);

			// get command
      int j = 0;
      for(int i = 0; i < TERM_MAX_COM_LEN; i++){
        if (buf[i] != 0) temp[j++] = buf[i];
      }
			// run command
      if(terminal_command(temp) == 0) ret_val = 0;

			// done for this round of input
      looper = 0;
    }
    else if((index < TERM_MAX_COM_LEN-1) && (c != 0)){
			putc(c);
      buf[index] = c;
      index++;
    }
  }
  return ret_val;
}





/*/////////////////////////////////////////////////////////////////////////////
/////////////////// MAIN FUNCTION: STARTS HERE ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////*/
void _main(multiboot_info_t* mbd, unsigned long kernel_end_addr, unsigned long kernel_start, unsigned int magic)
{
  // if magic number for multiboot is incorrect, halt. something don goofed
  if(magic != 0x2BADB002){
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


	printf("////////////////////////////////////////////////////////////////////////////////");
  printf("////////////////////////////// Kernel  initiated! //////////////////////////////");
	printf("////////////////////////////////////////////////////////////////////////////////\n");
	printf("Kernel size: %x \nKernel start address: %x \nKernel end address: %x.\n\n",
              kernel_end_addr - kernel_start, kernel_start, kernel_end_addr);



  // start reading multiboot info to get memory size and locations
  struct multiboot_mmap_entry *m_region = (struct multiboot_mmap_entry *) mbd->mmap_addr;
  // mem_lower is return in kilobytes and mem_upper is returned in a multiple of 64 kilobytes
  int phys_mem_size = mbd->mem_lower + (mbd->mem_upper * 64); // leave it in kilobytes because we want ints
  pmmngr_init(phys_mem_size, kernel_end_addr);

  pmap_address = (uint32_t) kernel_end_addr;
  pmap_size = pmmngr_get_block_count() / PMMNGR_BLOCKS_PER_BYTE;

  printf("PMEM Map at: %x, Length: %x\n\nMemory size is %x KB: Memory map returned by GRUB multiboot info:\n", pmap_address, pmap_size, phys_mem_size);

  // free those regions that grub said are free
  for(int i = 0; m_region[i].size > 0; i++){
    printf("Region %d: Address: %x, Length: %x, Type: %d.\n", i,
						(uint32_t) m_region[i].addr,
						(uint32_t) m_region[i].len,
						m_region[i].type);
    if(m_region[i].type == 1){
      pmmngr_init_region(m_region[i].addr, m_region[i].len);
    }
  }
  putc('\n');

  // set as used: memorymap used for physical_memory manager; kernel space;
  pmmngr_deinit_region(kernel_start, kernel_end_addr-kernel_start); // deep space, outer space, inner space, kernel space. I'm no astrophysicist
  pmmngr_deinit_region(kernel_end_addr, (phys_mem_size/PMMNGR_BLOCK_SIZE)/PMMNGR_BLOCKS_PER_BYTE); // space for physical memory manager


	// initiate virtual memory manager
  vmmngr_initialize();

	// initiate ATA handler
  ATA_init();

  printf("\nATA disk sector count: %d\n", (uint32_t) ATA_SECTOR_COUNT());


	// initiate fat file system
  fat_address = pmap_address + pmap_size;
  printf("\nAddress of FAT table: %x\n", fat_address);
  fat_init(fat_address);


	// start interrupts
  __asm__ __volatile__ ("sti");


	printf("\n\n////////////////////////////////////////////////////////////////////////////////"
	       "////////////////////////////////////////////////////////////////////////////////"
	       "--------------------------------------------------------------------------------"
				 "////////////////////////////////////////////////////////////////////////////////"
				 "////////////////////////////////////////////////////////////////////////////////"
				 "jaes16-OS-DEV:\nRudimentary operating system developed for educational purposes.\n"
				 "Type \"help\" to get a list of supported commands.\n\n");

	// start terminal
	volatile int main_looper = 1;
  while(main_looper){
		// no rest for the wicked
    if(start_input() == 0) main_looper = 0;
  }


	// halting...
  __asm__ __volatile__ ("cli");

  for (;;);
}


OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386:i386)

ENTRY(_start)
SECTIONS
{
  phys = 1M;
  . = phys;
  .text BLOCK(4K) : ALIGN(4K) {
    *(.multiboot)
    *(.text)
  }
  .rodata BLOCK(4K) : ALIGN(4K) {
    *(.rodata.*)
  }
  .data BLOCK(4K) : ALIGN(4K) {
    *(.data)
  }
  .bss BLOCK(4K) : ALIGN(4K) {
    *(COMMON)
    *(.bss)
  }

  phys_end = .;
}

/*

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386:i386)

ENTRY(_start)
SECTIONS
{
  phys = 1M;
  virt = 0xc0000000;


  . = virt;
  .text ALIGN(4K) : AT(phys) {
    *(.multiboot)
    *(.text)
  }
  phys_rodata = phys + (. - virt);
  .rodata ALIGN(4K) : AT(phys_rodata) {
    *(.rodata.*)
  }
  phys_data = phys + (. - virt);
  .data ALIGN(4K) : AT(phys_data) {
    *(.data)
  }
  phys_bss = phys + (. - virt);
  .bss ALIGN(4K) : AT(phys_bss) {
    *(COMMON)
    *(.bss)
  }

  phys_end = phys + (. - virt);
  virt_end = .;
}
*/

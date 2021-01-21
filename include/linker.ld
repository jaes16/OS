OUTPUT_FORMAT("elf32-i386")
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

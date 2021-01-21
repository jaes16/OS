.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# The kernel entry point.
.section .text
.global _start
.type _start, @function

_start:
  movl $stack_top, %esp

  # enable paging so that we can load kernel into virtual address of 3gb
  call _enable_paging

  ljmp $0x8, $0xc000001d

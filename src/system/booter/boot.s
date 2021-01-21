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

  # enable paging so that we can load kernel into virtual address of 3gb
  call _enable_paging

  ljmp $0x8, $0xc000001d

call_kernel:
  # Transfer control to the main kernel.
  movl $stack_top, %esp
  push %eax
  push $phys
  push $phys_end
  push %ebx
  call _main

halting:
  # Hang if kernel_main unexpectedly returns.
  cli
  hlt
	jmp halting

.size _start, . - _start
.long phys
.long phys_end


# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

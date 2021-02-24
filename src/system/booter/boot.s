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
.set OFFSET, call_kernel
.set CALL_KERNEL_ADDR, OFFSET+0xbff00000

_start:

  # enable paging so that we can run the kernel in the virtual address of 3gb
  call _enable_paging

  # long jump so that we jmp into higher half kernel. 0x8 is for the descriptor for the code seg]
  ljmp $0x8, $call_kernel

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
.skip 0x8000 # 32 KiB
stack_top:

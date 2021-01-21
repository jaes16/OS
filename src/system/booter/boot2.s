# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
  movl $0xCAFEDCAF, %eax
	movl $stack_top, %esp

  jmp stub

.align 4
mboot:
  # Multiboot macros to make a few lines later more readable
  .set MULTIBOOT_PAGE_ALIGN,	1<<0
  .set MULTIBOOT_MEMORY_INFO, 1<<1
  .set MULTIBOOT_AOUT_KLUDGE, 1<<16
  .set MULTIBOOT_HEADER_MAGIC, 0x1BADB002
  .set MULTIBOOT_HEADER_FLAGS, MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
  .set MULTIBOOT_CHECKSUM, -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

  # This is the GRUB Multiboot header. A boot signature
  .long MULTIBOOT_HEADER_MAGIC
  .long MULTIBOOT_HEADER_FLAGS
  .long MULTIBOOT_CHECKSUM

  # AOUT kludge - must be physical addresses. Make a note of these:
  # The linker script fills in the data for these ones!
  .long mboot
  .long code
  .long bss
  .long end
  .long _start

stub:
  # Transfer control to the main kernel.
  call _main

halting:
  # Hang if kernel_main unexpectedly returns.
  cli
  hlt
	jmp halting


# funciton to flush gdt and return using the code segment descriptor in gdt
.global gdt_flush
.type gdt_flush, @function
gdt_flush:
  movl $gp, %eax
  lgdt (%eax)
  mov $16, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs
  mov %ax, %ss
  ljmp $8, $flush2
flush2:
  ret


# Loads the IDT defined in '_idtp' into the processor.
# This is declared in C as 'extern void idt_load();'
.global idt_load
.type idt_load, @function
idt_load:
  mov $idtp, %eax
  lidt (%eax)
  ret

.global isr0
.type isr0, @function
.global isr1
.type isr1, @function
.global isr2
.type isr2, @function
.global isr3
.type isr3, @function
.global isr4
.type isr4, @function
.global isr5
.type isr5, @function
.global isr6
.type isr6, @function
.global isr7
.type isr7, @function
.global isr8
.type isr8, @function
.global isr9
.type isr9, @function
.global isr10
.type isr10, @function
.global isr11
.type isr11, @function
.global isr12
.type isr12, @function
.global isr13
.type isr13, @function
.global isr14
.type isr14, @function
.global isr15
.type isr15, @function
.global isr16
.type isr16, @function
.global isr17
.type isr17, @function
.global isr18
.type isr18, @function
.global isr19
.type isr19, @function
.global isr20
.type isr20, @function
.global isr21
.type isr21, @function
.global isr22
.type isr22, @function
.global isr23
.type isr23, @function
.global isr24
.type isr24, @function
.global isr25
.type isr25, @function
.global isr26
.type isr26, @function
.global isr27
.type isr27, @function
.global isr28
.type isr28, @function
.global isr29
.type isr29, @function
.global isr30
.type isr30, @function
.global isr31
.type isr31, @function

#  0: Divide By Zero Exception
isr0:
  cli
  push $0    # A normal ISR stub that pops a dummy error code to keep a uniform stack frame
  push $0
  jmp isr_common_stub
isr1:
  cli
  push $0
  push $1
  jmp isr_common_stub
isr2:
  cli
  push $0
  push $2
  jmp isr_common_stub
isr3:
  cli
  push $0
  push $3
  jmp isr_common_stub
isr4:
  cli
  push $0
  push $4
  jmp isr_common_stub
isr5:
  cli
  push $0
  push $5
  jmp isr_common_stub
isr6:
  cli
  push $0
  push $6
  jmp isr_common_stub
isr7:
  cli
  push $0
  push $7
  jmp isr_common_stub
isr8:
  cli
  # 8 has an error code, so an error code was already pushed
  push $8
  jmp isr_common_stub
isr9:
  cli
  push $0
  push $9
  jmp isr_common_stub
isr10:
  cli
  # 10 has an error code
  push $10
  jmp isr_common_stub
isr11:
  cli
  # 11 has an error code
  push $11
  jmp isr_common_stub
isr12:
  cli
  # 12 has an error code
  push $12
  jmp isr_common_stub
isr13:
  cli
  # 13 has an error code
  push $13
  jmp isr_common_stub
isr14:
  cli
  # 14 has an error code
  push $14
  jmp isr_common_stub
isr15:
  cli
  push $0
  push $15
  jmp isr_common_stub
isr16:
  cli
  push $0
  push $16
  jmp isr_common_stub
isr17:
  cli
  push $0
  push $17
  jmp isr_common_stub
isr18:
  cli
  push $0
  push $18
  jmp isr_common_stub
isr19:
  cli
  push $0
  push $19
  jmp isr_common_stub
isr20:
  cli
  push $0
  push $20
  jmp isr_common_stub
isr21:
  cli
  push $0
  push $21
  jmp isr_common_stub
isr22:
  cli
  push $0
  push $22
  jmp isr_common_stub
isr23:
  cli
  push $0
  push $23
  jmp isr_common_stub
isr24:
  cli
  push $0
  push $24
  jmp isr_common_stub
isr25:
  cli
  push $0
  push $25
  jmp isr_common_stub
isr26:
  cli
  push $0
  push $26
  jmp isr_common_stub
isr27:
  cli
  push $0
  push $27
  jmp isr_common_stub
isr28:
  cli
  push $0
  push $28
  jmp isr_common_stub
isr29:
  cli
  push $0
  push $29
  jmp isr_common_stub
isr30:
  cli
  push $0
  push $30
  jmp isr_common_stub
isr31:
  cli
  push $0
  push $31
  jmp isr_common_stub



# This is our common ISR stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level fault handler,
# and finally restores the stack frame.
isr_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0xcafedcaf, %eax
    xor %eax, %eax
    mov $0x10, %ax   # Load the Kernel Data Segment descriptor!
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %sp, %ax   # Push us the stack
    push %eax
    mov fault_handler, %eax
    call %eax       # A special call, preserves the 'eip' register
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!


.global irq0
.type irq0, @function
.global irq1
.type irq1, @function
.global irq2
.type irq2, @function
.global irq3
.type irq3, @function
.global irq4
.type irq4, @function
.global irq5
.type irq5, @function
.global irq6
.type irq6, @function
.global irq7
.type irq7, @function
.global irq8
.type irq8, @function
.global irq9
.type irq9, @function
.global irq10
.type irq10, @function
.global irq11
.type irq11, @function
.global irq12
.type irq12, @function
.global irq13
.type irq13, @function
.global irq14
.type irq14, @function
.global irq15
.type irq15, @function

irq0:
  cli
  push $0
  push $32
  jmp irq_common_stub
irq1:
  cli
  push $1
  push $33
  jmp irq_common_stub
irq2:
  cli
  push $2
  push $34
  jmp irq_common_stub
irq3:
  cli
  push $3
  push $35
  jmp irq_common_stub
irq4:
  cli
  push $4
  push $36
  jmp irq_common_stub
irq5:
  cli
  push $5
  push $37
  jmp irq_common_stub
irq6:
  cli
  push $6
  push $38
  jmp irq_common_stub
irq7:
  cli
  push $7
  push $39
  jmp irq_common_stub
irq8:
  cli
  push $8
  push $40
  jmp irq_common_stub
irq9:
  cli
  push $9
  push $41
  jmp irq_common_stub
irq10:
  cli
  push $10
  push $42
  jmp irq_common_stub
irq11:
  cli
  push $11
  push $43
  jmp irq_common_stub
irq12:
  cli
  push $12
  push $44
  jmp irq_common_stub
irq13:
  cli
  push $13
  push $45
  jmp irq_common_stub
irq14:
  cli
  push $14
  push $46
  jmp irq_common_stub
irq15:
  cli
  push $15
  push $47
  jmp irq_common_stub


irq_common_stub:
  pusha
  push %ds
  push %es
  push %fs
  push %gs
  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs
  mov %esp, %eax
  push %eax
  mov irq_handler, %eax
  call %eax
  pop %eax
  pop %gs
  pop %fs
  pop %es
  pop %ds
  popa
  add $8, %esp
  iret


.size _start, . - _start



# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

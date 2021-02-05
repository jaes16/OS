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
    xor %eax, %eax
    mov $0x10, %ax   # 16 = kernel data segment descriptor (after empty and code segment descript.)
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax   # Push us the stack. make sure we're moving the whole register. else 0xc0001000 might become 0x1000
    push %eax
    call fault_handler
    # mov fault_handler, %eax
    # call %eax       # A special call, preserves the 'eip' register
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

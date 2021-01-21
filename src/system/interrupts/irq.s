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
  # mov irq_handler, %eax
  # call %eax
  call irq_handler
  pop %eax
  pop %gs
  pop %fs
  pop %es
  pop %ds
  popa
  add $8, %esp
  iret

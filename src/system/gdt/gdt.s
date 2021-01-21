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

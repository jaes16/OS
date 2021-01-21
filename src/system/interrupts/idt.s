# Loads the IDT defined in '_idtp' into the processor.
# This is declared in C as 'extern void idt_load();'
.global idt_load
.type idt_load, @function
idt_load:
  mov $idtp, %eax
  lidt (%eax)
  ret

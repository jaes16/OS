; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
 section .text

global _start
_start:
	mov esp, stack_top

	extern _main
	call _main

	cli
.hang:	hlt
	jmp .hang
.end:


 ; This will set up our new segment registers. We need to do
 ; something special in order to set CS. We do what is called a
 ; far jump. A jump that includes a segment as well as an offset.
 ; This is declared in C as 'extern void gdt_flush();'
global _gdt_flush     ; Allows the C code to link to this
extern _gp            ; Says that '_gp' is in another file
_gdt_flush:
  lgdt [_gp]        ; Load the GDT with our '_gp' which is a special pointer
  mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp 0x08:flush2   ; 0x08 is the offset to our code segment: Far jump!
flush2:
  ret               ; Returns back to the C code!

  

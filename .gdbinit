# source file:
file build/kernel

# Intel syntax
set disassembly-flavor intel
set architecture i386:intel

# connect to qemu port
target remote localhost:26000

# break at point where grub loads us in
b *0x10000c
# break at main
b _main
# we should probably find out why we faulted
b fault_handler

c

layout split

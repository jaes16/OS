#define hook-stop
  # Translate the segment:offset into a physical address
#  printf "[%4x:%4x] ", $cs, $eip
#  x/i $cs*16+$eip
#end

file build/kernel

# Intel syntax
set disassembly-flavor intel

set architecture i386:intel

# connect to qemu port
target remote localhost:26000

b _start
b _main

c

layout split

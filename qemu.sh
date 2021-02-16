#qemu-system-i386 -kernel ./build/kernel -gdb tcp::26000 -S
qemu-system-i386 -kernel ./build/kernel -hda ./build/osdev.img

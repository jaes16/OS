#qemu-system-i386 -kernel ./build/kernel -hda ./build/disk1.img -hdb ./build/disk2.img -gdb tcp::26000 -S
qemu-system-i386 -kernel ./build/kernel -hda ./build/disk1.img -hdb ./build/disk2.img

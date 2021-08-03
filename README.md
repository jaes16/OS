**OS_dev**
=====
 	**Architecture:** x86

 	**Requirement for running:**
	- qemu-system-i386
	- i686-elf-gcc
	- i386-elf-gdb
	- GRUB

 	**Running the OS:**
	Create a `build` directory and call `make` to create the kernel
	Call `qemu-createimg.sh` to create disk images
 	Run qemu with `qemu-system-i386 -kernel ./build/kernel -hda ./build/disk1.img`, or run `./qemu.sh`.
	To run with a connection to the gdb debugger, run with `qemu-system-i386 -kernel ./build/kernel -hda ./build/disk1.img  -gdb tcp::26000 -S`, or run `./qemu-gdb.sh`.

	**Supported OS terminal commands:**
	```
	echo
	time
	readdisk
	readdisk-hex
	writedisk
	ls
	cd
	mkdir
	rmdir
	mknod
	rm
	less
	hexdump
	shutdown
	help
	```

 	**Needs more work:**

 	- gdt
 	- interrupts
 	- keyboard input
 	- VGA text output
 	- timer
 	- virtual memory manager

 	**To do:**

 	- User space
 	- Process Management
 	- C Library

 	**Referenced tutorials from:** [Bran's Kernel Development Tutorial](http://www.osdever.net/bkerndev/Docs/intro.htm), [Bare Bones Tutorial](https://wiki.osdev.org/Bare_Bones), [BrokenThorn: OS Development Series](http://www.brokenthorn.com/Resources/OSDevIndex.html), [Little OS Book](https://littleosbook.github.io/), [AlgorithMan-de/wyoos](https://github.com/AlgorithMan-de/wyoos)

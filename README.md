**OS_dev**
=====
## Overview:
Runs on x86, for now with a qemu virtual machine. The purpose of this project is mainly educational, to teach myself OS development, as well as being an interesting side hobby. The project is still very rough around the edges, but provides very rudimentary memory management, a FATFS-style file system, and a terminal to interact with.

## Requirements for running:
- [qemu-system-i386](https://www.qemu.org/)
- [i386-elf-gcc](https://github.com/nativeos/homebrew-i386-elf-toolchain)
- [i386-elf-gdb](https://formulae.brew.sh/formula/i386-elf-gdb#default) (suggested)
- [GRUB](https://www.gnu.org/software/grub/)

If on Mac or Linux, these can easily be installed with homebrew. [Cross-compiling is necessary.](https://wiki.osdev.org/GCC_Cross-Compiler)

## Running the OS:

1. Create a `/build` directory and call `make` to create the kernel.
2. Call `qemu-createimg.sh` to create disk images.
3. Run qemu with `qemu-system-i386 -kernel ./build/kernel -hda ./build/disk1.img`, or run `qemu.sh`.
4. To run with a connection to the gdb debugger, run with `qemu-system-i386 -kernel ./build/kernel -hda ./build/disk1.img  -gdb tcp::26000 -S`, or run `qemu-gdb.sh`.

## Supported OS terminal commands:
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

## Needs more work:
- gdt
- interrupts
- keyboard input
- VGA text output
- timer
- virtual memory manager
- FAT file system

## To do:
- Malloc
- User space
- Process Management
- C Library
- Loads more

## Referenced tutorials from:
[Bran's Kernel Development Tutorial](http://www.osdever.net/bkerndev/Docs/intro.htm), [Bare Bones Tutorial](https://wiki.osdev.org/Bare_Bones), [BrokenThorn: OS Development Series](http://www.brokenthorn.com/Resources/OSDevIndex.html), [Little OS Book](https://littleosbook.github.io/), [AlgorithMan-de/wyoos](https://github.com/AlgorithMan-de/wyoos)

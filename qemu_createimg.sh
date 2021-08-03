rm build/disk1.img
rm build/disk2.img

qemu-img create -f raw build/disk1.img 200M
qemu-img create -f raw build/disk2.img 200M

# recreate disk images for qemu, 

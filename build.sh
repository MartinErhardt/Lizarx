#build shell skript
rm bootable.iso
cd src
make 
echo "compiled it succesfully"
rm init.o
rm start.o
rm console.o
rm gdt.o
rm idt.o
rm int_stub.o
rm ioport.o
rm vram.o

cd lib
rm string.o
rm stdlib.o

cd ..
cd intr
rm irq.o
rm redscreen.o
rm syscall.o

cd ..
cd driver
cd keyboard
rm keyboard.o
echo "deleted .o files"

cd ..
cd ..
cd ..
cp src/kernel bin/boot
rm src/kernel
genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootable.iso bin
/usr/bin/qemu-system-x86_64 -cdrom ~/git/el_toro_repo/bootable.iso -d int -no-kvm

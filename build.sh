#build shell skript
echo "This is the build-script for lizarx86"
echo "It's required ro run in the root folder of the project"

rm bootable.iso

cd src
make 
echo "compiled it succesfully"

cd kernel
#rm kernel.elf

cd lib
rm *.o

cd ../intr
rm *.o

cd ../mt
rm *.o

cd ../boot
rm *.o

cd ../mm
rm *.o

cd ../dbg
rm *.o

cd ../HAL/x86/asm
rm *.o

cd ../../../drv/keyboard
rm *.o

cd ../vga-txt_graphics
rm *.o

cd ../timer
rm *.o

cd ../../../usr/tst
rm *.o

echo "deleted .o files"

cd ../../..
cp src/kernel/kernel bin/boot/kernel
rm src/kernel/kernel
cp src/usr/tst/test.bin bin/boot/test.mod
genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o lizarx86.iso bin
/usr/bin/qemu-system-x86_64 -cdrom lizarx86.iso -d int -no-kvm -d int

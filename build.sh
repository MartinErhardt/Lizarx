#build shell skript
echo "This is the build-script for lizarx86"
echo "It's required ro run in the root folder of the project"

rm lizarx86.iso

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

cd ../hwtime
rm *.o

cd ../timer
rm *.o

cd ../../../usr/tst1
rm *.o

cd ../tst2
rm *.o

cd ../tst3
rm *.o

echo "deleted .o files"

cd ../../..
pwd
cp src/kernel/kernel bin/boot/kernel
rm src/kernel/kernel
cp src/usr/tst1/tst1.elf bin/boot/test1.mod
cp src/usr/tst2/tst2.elf bin/boot/test2.mod
cp src/usr/tst3/tst3.elf bin/boot/test3.mod
genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o lizarx86.iso bin
/usr/bin/qemu-system-x86_64 -cdrom lizarx86.iso -d int -no-kvm -d int

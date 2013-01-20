#build shell skript
rm bootable.iso
cd src
make 
echo "compiled it succesfully"
rm *.o

cd lib
rm *.o

cd ../intr
rm *.o

cd ../mt
rm *.o

cd ../boot
rm *.o

cd ../drv/keyboard
rm *.o

cd ../timer
rm *.o
echo "deleted .o files"

cd ../../..
cp src/kernel bin/boot
rm src/kernel
genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootable.iso bin
/usr/bin/qemu-system-x86_64 -cdrom ~/git/el_toro_repo/bootable.iso -d int -no-kvm -d int

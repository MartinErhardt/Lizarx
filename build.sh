#build shell skript
echo "This is the build-script for lizarx86"
echo "It's required ro run in the root folder of the project"

rm lizarx86.iso


if [ ! -f bin/boot/grub/stage2_eltorito ]
then
	echo "downloading stage2_eltorito"
	cd bin/boot/grub/stage2_eltorito
	wget https://docs.google.com/file/d/0B-x3QNiQfEeCallWMmxzVlZPRHM/edit
	mv edit stage2_eltorito
	cd ../../..
fi

if [ ! -d src/usr/buildtools ]
then
	echo "downloading buildutils"
	cd src/usr
	#git clone https://github.com/MartinErhardt/buildtools.git
	chmod buildtools 771
	cd ../..
fi

cd src
make 
cd ..

cp ./src/kernel/kernel ./bin/boot/kernel
rm ./src/kernel/kernel
cp ./src/usr/tst1/tst1.elf ./bin/boot/test1.mod
cp ./src/usr/tst2/tst2.elf ./bin/boot/test2.mod
cp ./src/usr/tst3/tst3.elf ./bin/boot/test3.mod
genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o lizarx86.iso bin
/usr/bin/qemu-system-x86_64 -cdrom lizarx86.iso -d int -no-kvm -d int 

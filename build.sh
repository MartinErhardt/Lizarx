#build shell skript
echo "This is the build-script for lizarx"
echo "It's required ro run in the root folder of the project"
rm -f *.iso
rm -rf bin/boot
mkdir -p bin/boot
mkdir -p bin/boot/grub
mkdir -p sysroot
cp bin/stage2_eltorito bin/boot/grub/stage2_eltorito
TOOLCHAIN_DIR=host
CLONE_TOOLCHAIN="FALSE"
CUR_DIR=$(PWD)
export PROJ_ROOT=$(pwd)
if [ "$1" == "--arch-x86_64" ] || [ "$2" == "--arch-x86_64" ]
then
	export ARCH="x86_64"
	export ARCH2="x86_64"
	#export _64_EXTRA=-lsupc++_pic -lstdc++_pic
	cp src/kernel/HAL/x86_64/makefile 	src/kernel/makefile
	cp src/kernel/HAL/x86_64/kernel.ld 	src/kernel/kernel.ld
	cp src/kernel/HAL/x86_64/loader.ld 	src/kernel/loader.ld
	
	cp src/usr/arch/x86_64/archdef.h 	src/usr
	cp src/usr/arch/x86_64/asm_inline.h 	src/usr
else
	export ARCH="i386"
	export ARCH2="x86"
	
	cp src/kernel/HAL/x86/makefile 		src/kernel/makefile
	cp src/kernel/HAL/x86/kernel.ld 	src/kernel/kernel.ld
	
	cp src/usr/arch/x86/archdef.h 		src/usr
	cp src/usr/arch/x86/asm_inline.h 	src/usr
fi

if [ "$1" != "--no-toolchain" ] && [ "$2" != "--no-toolchain" ] 
then
	
	cd $TOOLCHAIN_DIR
	export TARGET=$ARCH-pc-lizarx
	./build.sh $TARGET
	cd ..
fi
export COMPILER_PATH=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-gcc
export PATH=$PATH:$COMPILER_PATH
export LD_CROSS=$(pwd)/$TOOLCHAIN_DIR/toolchain/$ARCH-pc-lizarx/$ARCH-pc-lizarx/bin/ld
export LD32_CROSS=$(pwd)/$TOOLCHAIN_DIR/toolchain/i386-pc-lizarx/bin/i386-pc-lizarx-ld
export CC_CROSS=$(pwd)/$TOOLCHAIN_DIR/toolchain/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-gcc
export CPPC_CROSS=$(pwd)/$TOOLCHAIN_DIR/toolchain/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-g++
export AR_CROSS=$(pwd)/$TOOLCHAIN_DIR/toolchain/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-ar
export INCLUDEDIR=$(pwd)/$TOOLCHAIN_DIR/toolchain/$ARCH-pc-lizarx/$ARCH-pc-lizarx/include
export LIBDIR=$(pwd)/$TOOLCHAIN_DIR/toolchain/$ARCH-pc-lizarx/$ARCH-pc-lizarx/lib
export COMPILER_PATH=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-gcc
export CPP_EXTRA="-fno-rtti -fno-exceptions"

cd src
make || exit
cd ..

mv ./src/kernel/kernel 			./bin/boot/kernel
if [ "$ARCH" == "x86_64" ]
then
	mv ./src/kernel/LM_Loader 		./bin/boot/LM_Loader
fi
cp ./src/kernel/HAL/$ARCH2/menu.lst 	./bin/boot/grub/menu.lst
#mv ./src/usr/tst2/SO_example_main.elf 	./bin/boot/SO_example_main.mod
#mv ./src/usr/tst2/libSO_example_lib.so 	./bin/boot/SO_example_lib.mod
#mv ./src/usr/tst1/tst1.elf 		./bin/boot/proc1.mod
mv ./src/usr/VFS_unit_test/tst1.elf 	./bin/boot/vfs_test.mod
#mv ./src/usr/tst4/tst1.elf 		./bin/boot/proc3.mod
mv ./src/usr/Daemons/VFS/vfs.elf	./bin/boot/vfs.mod
mv ./src/usr/Daemons/VFS/FS/tmpFS/libtmpfs.so ./bin/boot/tmpfs.mod
cd sysroot
tar -cvf ../bin/initrd.tar .
cd ..
genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o lizarx-$ARCH2.iso bin
#/usr/bin/qemu-system-x86_64 -cdrom lizarx-$ARCH2.iso -smp 4 -cpu core2duo #-d int
#date | cat >> ./doc/lines_of_code.txt
#(cloc . --exclude-dir=./host | cat >> ./doc/lines_of_code.txt)&exit

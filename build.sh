#build shell skript
echo "This is the build-script for lizarx86"
echo "It's required ro run in the root folder of the project"
rm lizarx86.iso
TOOLCHAIN_DIR=host
CLONE_TOOLCHAIN="FALSE"
ARCH="i386"

F_ARCH_X86_64="--arch-x86_64"
F_NO_TOOLCHAIN="--no-toolchain"
F_ARCH="i386"

export PROJ_ROOT=$(pwd)
if [ ! -d $TOOLCHAIN_DIR/buildtools ]
then
	CLONE_TOOLCHAIN="TRUE"
fi
if [ "$2"=="--no-toolchain" ]
then
	echo "1 argument given"
fi
if [ "$1" == $F_ARCH_X86_64 ] || [ "$2" == $F_ARCH_X86_64 ]
then
	ARCH="x86_64"
	F_ARCH=$F_ARCH_X86_64
	cp src/kernel/HAL/x86_64/Makefile src/kernel/Makefile
	cp src/kernel/HAL/x86_64/kernel.ld src/kernel/kernel.ld
	cp src/kernel/HAL/x86_64/archdef.h src/kernel/HAL/archdef.h

else
	cp src/kernel/HAL/x86/Makefile src/kernel/Makefile
	cp src/kernel/HAL/x86/kernel.ld src/kernel/kernel.ld
	cp src/kernel/HAL/x86/archdef.h src/kernel/HAL/archdef.h
fi

if [ "$1" != $F_NO_TOOLCHAIN ] && [ "$2" != $F_NO_TOOLCHAIN ]
then
	
	cd $TOOLCHAIN_DIR
	if [ $CLONE_TOOLCHAIN == "TRUE" ]
	then
		git clone https://github.com/MartinErhardt/buildtools.git
	fi
	cd buildtools
	./build.sh $F_ARCH
	cd ../..
fi

export COMPILER_PATH=$(pwd)/$TOOLCHAIN_DIR/buildtools/build/gcc-obj/gcc
export PATH=$PATH:$COMPILER_PATH
export LD_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/local/$ARCH-pc-lizarx/bin/ld
export CC_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/local/bin/$ARCH-pc-lizarx-gcc
export CPPC_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/local/bin/$ARCH-pc-lizarx-g++
export AR_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/local/bin/$ARCH-pc-lizarx-ar

if [ ! -f bin/boot/grub/stage2_eltorito ]
then
	echo "downloading stage2_eltorito"
	cd bin/boot/grub/
	wget https://docs.google.com/file/d/0B-x3QNiQfEeCallWMmxzVlZPRHM/edit
	mv edit stage2_eltorito
	cd ../../..
fi

cd src
make 
cd ..

cp ./src/kernel/kernel ./bin/boot/kernel
rm ./src/kernel/kernel
mv ./src/usr/tst2/SO_example_main.elf ./bin/boot/SO_example_main.mod
mv ./src/usr/tst2/libSO_example_lib.so ./bin/boot/SO_example_lib.mod
mv ./src/usr/tst1/tst1.elf ./bin/boot/proc1.mod
genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o lizarx86.iso bin
/usr/bin/qemu-system-x86_64 -cdrom lizarx86.iso -d int -no-kvm -d int 

#build shell skript
echo "This is the build-script for lizarx86"
echo "It's required ro run in the root folder of the project"
rm lizarx86.iso
TOOLCHAIN_DIR=host
CLONE_TOOLCHAIN="FALSE"
export ARCH="i386"
export PROJ_ROOT=$(pwd)
if [ ! -d $TOOLCHAIN_DIR/buildtools ]
then
	CLONE_TOOLCHAIN="TRUE"
fi
if [ "$2"=="--no-toolchain" ]
then
	echo "1 argument given"
fi
if [ "$1" == "--arch-x86_64" ] || [ "$2" == "--arch-x86_64" ]
then
	R_ARCH="x86_64"
	cp src/kernel/HAL/x86_64/Makefile src/kernel/Makefile
	cp src/kernel/HAL/x86_64/kernel.ld src/kernel/kernel.ld
	cp src/kernel/HAL/x86_64/loader.ld src/kernel/loader.ld
	cp src/kernel/HAL/x86_64/archdef.h src/kernel/HAL/archdef.h
else
	cp src/kernel/HAL/i386/Makefile src/kernel/Makefile
	cp src/kernel/HAL/i386/kernel.ld src/kernel/kernel.ld
	cp src/kernel/HAL/i386/archdef.h src/kernel/HAL/archdef.h
fi

if [ "$1" != "--no-toolchain" ] && [ "$2" != "--no-toolchain" ]
then
	
	cd $TOOLCHAIN_DIR
	if [ $CLONE_TOOLCHAIN == "TRUE" ]
	then
		git clone https://github.com/MartinErhardt/buildtools.git
	fi
	export TARGET=i386-pc-lizarx
	cd buildtools
	#./build.sh
	if [ "$R_ARCH" == "x86_64" ]
	then
		export ARCH=$R_ARCH
		export TARGET=x86_64-pc-lizarx
		./build.sh
	fi
	cd ../..
fi
if [ "$R_ARCH" == "x86_64" ]
then
	export ARCH=$R_ARCH
	export TARGET=x86_64-pc-lizarx
fi
export COMPILER_PATH=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-gcc
export PATH=$PATH:$COMPILER_PATH
export LD_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/$ARCH-pc-lizarx/bin/ld
export LD32_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/i386-pc-lizarx/i386-pc-lizarx/bin/ld
export CC_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-gcc
export CPPC_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-g++
export AR_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-ar

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

mv ./src/kernel/kernel ./bin/boot/kernel
mv ./src/kernel/LM_Loader ./bin/boot/LM_Loader
cp ./src/kernel/HAL/$ARCH/menu.lst ./bin/boot/grub/menu.lst
mv ./src/usr/tst2/SO_example_main.elf ./bin/boot/SO_example_main.mod
mv ./src/usr/tst2/libSO_example_lib.so ./bin/boot/SO_example_lib.mod
mv ./src/usr/tst1/tst1.elf ./bin/boot/proc1.mod

genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o lizarx-$ARCH.iso bin
/usr/bin/qemu-system-x86_64 -cdrom lizarx-$ARCH.iso -d int  -no-kvm -d int
date | cat >> ./doc/lines_of_code.txt
(cloc . --exclude-dir=host/buildtools --exclude-list-file=doc/exclude_cloc.txt | cat >> ./doc/lines_of_code.txt)&exit

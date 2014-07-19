#build shell skript
echo "This is the build-script for lizarx86"
echo "It's required ro run in the root folder of the project"
rm *.iso
TOOLCHAIN_DIR=host
CLONE_TOOLCHAIN="FALSE"
export ARCH="i386"
export ARCH2="x86"
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
	cp src/kernel/HAL/x86_64/makefile 	src/kernel/makefile
	cp src/kernel/HAL/x86_64/kernel.ld 	src/kernel/kernel.ld
	cp src/kernel/HAL/x86_64/loader.ld 	src/kernel/loader.ld
	
	cp src/usr/arch/x86_64/archdef.h 	src/usr
	cp src/usr/arch/x86_64/asm_inline.h 	src/usr
else
	cp src/kernel/HAL/x86/makefile 		src/kernel/makefile
	cp src/kernel/HAL/x86/kernel.ld 	src/kernel/kernel.ld
	
	cp src/usr/arch/x86/archdef.h 		src/usr
	cp src/usr/arch/x86/asm_inline.h 	src/usr
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
	if [ "$R_ARCH" == "x86_64" ]
	then
		export ARCH=$R_ARCH
		export TARGET=x86_64-pc-lizarx
		./build.sh
	else
		./build.sh
	fi
	cp $(pwd)/$TOOLCHAIN_DIR/buildtools/build/newlib-obj/$ARCH-pc-lizarx/newlib/crt0.o $(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/crt0.o
	cd ../..
fi
cp $(pwd)/$TOOLCHAIN_DIR/buildtools/build/newlib-obj/$ARCH-pc-lizarx/newlib/crt0.o $(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/crt0.o
if [ "$R_ARCH" == "x86_64" ]
then
	export ARCH=$R_ARCH
	export ARCH2=$R_ARCH
	export TARGET=x86_64-pc-lizarx
fi
export COMPILER_PATH=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-gcc
export PATH=$PATH:$COMPILER_PATH
export LD_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-ld
export LD32_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/i386-pc-lizarx/bin/i386-pc-lizarx-ld
export CC_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-gcc
export CPPC_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-g++
export AR_CROSS=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/$ARCH-pc-lizarx-ar
export LD_CRT0_FLG=$(pwd)/$TOOLCHAIN_DIR/buildtools/$ARCH-pc-lizarx/bin/crt0.o
echo $CPPC_CROSS
echo $LD_CROSS
echo $LD_CRT0_FLG
cd src
make
cd ..

mv ./src/kernel/kernel 			./bin/boot/kernel
mv ./src/kernel/LM_Loader 		./bin/boot/LM_Loader
cp ./src/kernel/HAL/$ARCH2/menu.lst 	./bin/boot/grub/menu.lst
mv ./src/usr/tst2/SO_example_main.elf 	./bin/boot/SO_example_main.mod
mv ./src/usr/tst2/libSO_example_lib.so 	./bin/boot/SO_example_lib.mod
mv ./src/usr/tst1/tst1.elf 		./bin/boot/proc1.mod
mv ./src/usr/tst3/tst1.elf 		./bin/boot/proc2.mod
mv ./src/usr/tst4/tst1.elf 		./bin/boot/proc3.mod

genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o lizarx-$ARCH2.iso bin
#/usr/bin/qemu-system-x86_64 -cdrom lizarx-$ARCH2.iso -smp 4 -cpu core2duo #-d int
date | cat >> ./doc/lines_of_code.txt
(cloc . --exclude-dir=host/buildtools --exclude-list-file=doc/exclude_cloc.txt | cat >> ./doc/lines_of_code.txt)&exit

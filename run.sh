#genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootable.iso bin
DBG_FLG="-d int"
QEMU=host/toolchain/host/bin/qemu-system-x86_64
rm -f ../harddrive.img
touch ../harddrive.img
if  [ -f lizarx-x86.iso ]
then
	ISO="lizarx-x86.iso"
fi
if [ -f lizarx-x86_64.iso ]
then
	ISO="lizarx-x86_64.iso"
fi
if [ "$1" == "--dbg" ]
then
	( $QEMU -cdrom $ISO -smp 2 -cpu core2duo -s -S ../harddrive.img)&gdb64
else
	$QEMU -cdrom $ISO -smp 4 -d int #-curses
fi

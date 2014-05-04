#genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootable.iso bin
if  [ -f lizarx-x86.iso ]
then
	/usr/bin/qemu-system-x86_64 -cdrom lizarx-x86.iso -smp 4  -cpu core2duo -d int
fi
if [ -f lizarx-x86_64.iso ]
then
	/usr/bin/qemu-system-x86_64 -cdrom lizarx-x86_64.iso -smp 4  -cpu core2duo  -no-kvm # -d int
fi


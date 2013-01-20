genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootable.iso bin
/usr/bin/qemu-system-x86_64 -cdrom ~/git/el_toro_repo/bootable.iso -d int -no-kvm -d int

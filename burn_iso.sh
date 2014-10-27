CDRW_DEVICE=$1
if  [ -f lizarx-x86.iso ]
then
        ISO="lizarx-x86.iso"
fi
if [ -f lizarx-x86_64.iso ]
then
        ISO="lizarx-x86_64.iso"
fi
cdrecord -v -eject blank=fast -tao dev=$CDRW_DEVICE $ISO

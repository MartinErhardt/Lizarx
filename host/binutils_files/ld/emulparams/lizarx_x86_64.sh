SCRIPT_NAME=elf
ELFSIZE=64
OUTPUT_FORMAT=elf64-x86-64
#NO_REL_RELOCS=yes
TEXT_START_ADDR=0x50000000
MAXPAGESIZE="CONSTANT (MAXPAGESIZE)"
COMMONPAGESIZE="CONSTANT (COMMONPAGESIZE)"
ARCH=i386:x86-64
MACHINE=
NOP=0x90909090
TEMPLATE_NAME=elf32
GENERATE_SHLIB_SCRIPT=yes
GENERATE_PIE_SCRIPT=yes
NO_SMALL_DATA=yes
LARGE_SECTIONS=yes
SEPARATE_GOTPLT=24

#if [ "x${host}" = "x${target}" ]; then
#  case " $EMULATION_LIBPATH " in
#    *" ${EMULATION_NAME} "*)
#      NATIVE=yes
#  esac
#fi

# defines the scripttempl file to use, in this case the ELF one
SCRIPT_NAME=elf
 
# Create our executables in elf32 format.
OUTPUT_FORMAT=elf32-i386
 
# TODO: Find a good default value to recommend.
# TODO: Document.
#NO_RELA_RELOCS=yes
 
# TODO: Find a good default value to recommend.
# Start of the .text section and therefore the entire executable image.  I set to this because my kernel occupies 0x0-0x3fffffff.
TEXT_START_ADDR=0x10000000
 
# Tell ld the page sizes so it can properly align sections to page boundaries. Just use the defaults here.
MAXPAGESIZE="CONSTANT (MAXPAGESIZE)"
COMMONPAGESIZE="CONSTANT (COMMONPAGESIZE)"
 
# Self explanatory.
ARCH=i386
MACHINE=
 
# What ld pads sections with, basically the i386 NOP instruction 4 times to fill a 32-bit value.
NOP=0x90909090
 
# Defines the emultempl file to use, again, stick with ELF.
TEMPLATE_NAME=elf32
 
# Should we generate linker scripts to produced shared libraries and position-independent executables respectively.
GENERATE_SHLIB_SCRIPT=yes
GENERATE_PIE_SCRIPT=yes
 
# Unsure as to these, just copied from the ELF one, seems to work okay.
# TODO: Find a good default value to recommend.
# TODO: Document.
NO_SMALL_DATA=yes
SEPARATE_GOTPLT=12

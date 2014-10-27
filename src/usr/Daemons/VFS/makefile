VFS_SRCS =	main.cpp \
	vfs.cpp
VFS_OBJS = $(addsuffix .o,$(basename $(VFS_SRCS)))
all: FS vfs.elf _clean
FS:
	$(MAKE) -C FS
vfs.elf: $(VFS_OBJS)
	$(CPPC_CROSS) -o $@ $^ -L../../hlib -lhelper -L../../ld_a -ldld -dynamic -LFS/tmpFS -ltmpfs
%.o: %.cpp
	$(CPPC_CROSS) -c -o $@ $^ -Wall -I ../../ld_a -I ../../hlib -I ../../arch/$(ARCH2) $(CPP_EXTRA)
_clean:
	rm $(VFS_OBJS)
.PHONY: FS all _clean

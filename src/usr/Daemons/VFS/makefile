VFS_SRCS =	main.cpp \
	vfs.cpp
VFS_OBJS = $(addsuffix .o,$(basename $(VFS_SRCS)))
all: FS vfs.elf clean
FS:
	$(MAKE) -C FS
vfs.elf: $(VFS_OBJS)
	$(CPPC_CROSS) -o $@ $^ -L../../hlib -lhelper 
%.o: %.cpp
	$(CPPC_CROSS) -c -o $@ $^ -Wall -I ../../hlib -I ../../arch/$(ARCH2) $(CPP_EXTRA)
clean:
	rm $(VFS_OBJS)

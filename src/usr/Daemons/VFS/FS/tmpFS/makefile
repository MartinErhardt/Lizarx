TMPFS_SRCS = tmpfs.cpp
TMPFS_OBJS = $(addsuffix .o,$(basename $(TMPFS_SRCS)))
all: libtmpfs.so clean
libtmpfs.so: $(TMPFS_OBJS)
	$(CPPC_CROSS) -shared -o $@  $^ -nodefaultlibs -fno-rtti -fno-exceptions -I../../../../hlib -L../../../../hlib -fno-rtti -fno-exceptions -lhelper_pic
%.o: %.cpp
	$(CPPC_CROSS) -fpic -I../../../../hlib -I../../../../arch/$(ARCH2) -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow -fno-rtti -fno-exceptions -o $@ -c $^ -O3
clean:
	rm $(TMPFS_OBJS)

TMPFS_SRCS = $(shell find -name '*.c')
TMPFS_OBJS = $(addsuffix .o,$(basename $(SRCS)))
all: tmpfs.so clean
tmpfs.so: $(TMPFS_OBJS)
	$(CPPC_CROSS) -shared -o $@ $^ -I../../../../hlib -lhelper
%.o %.cpp
	$(CPPC_CROSS) -fpic $^ -c $(CPP_EXTRA) 
clean:
	rm $(TMPFS_OBJS)

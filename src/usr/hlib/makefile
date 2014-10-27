SRCS_HLIB = 	list.c \
		dbg.c \
		hlib_vfs.c\
		vmm.c\
		get_pid.c\
		multiboot_modules.c \
		OO/AList.cpp\
		hlib_ipc.cpp\
		OO/sem.cpp
OBJS_HLIB = $(addsuffix .o,$(basename $(SRCS_HLIB)))
OBJS_HLIBPIC=$(addsuffix .opic,$(basename $(SRCS_HLIB)))
CFLAGS=-I ../arch/$(ARCH2) -Wall 
CPP_FLAGS=$(CFLAGS)   -fno-exceptions
all: hlib.a hlib_pic.a clean 
hlib.a: $(OBJS_HLIB)
	rm -f libhelper.a
	$(AR_CROSS) rcs libhelper.a $(OBJS_HLIB)
hlib_pic.a: $(OBJS_HLIBPIC)
	rm -f libhelper_pic.a
	$(AR_CROSS) rcs libhelper_pic.a $(OBJS_HLIBPIC)
clean:
	rm $(OBJS_HLIB) $(OBJS_HLIBPIC)
%.o: %.c
	$(CPPC_CROSS) $(CFLAGS) -o $@ -c $<
	$(CPPC_CROSS) $(CFLAGS) -o $@pic -c $< -DPIC -fPIC
%.o: %.cpp
	$(CPPC_CROSS) $(CPP_FLAGS) -o $@ -c $<
	$(CPPC_CROSS) $(CPP_FLAGS) -o $@pic -c $< -DPIC -fPIC

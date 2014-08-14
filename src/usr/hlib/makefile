SRCS_HLIB = 	list.c \
		OO/AList.cpp\
		OO/ipc_helper.cpp\
		dbg.c
OBJS_HLIB = $(addsuffix .o,$(basename $(SRCS_HLIB)))
CFLAGS=-I ../arch/$(ARCH2) -Wall 
CPP_FLAGS=$(CFLAGS)   -fno-exceptions
hlib.a: $(OBJS_HLIB)
	rm libhelper.a
	$(AR_CROSS) rcs libhelper.a $(OBJS_HLIB)
	rm $(OBJS_HLIB)
%.o: %.c
	$(CC_CROSS) $(CFLAGS)-o $@ -c $<
%.o: %.cpp
	$(CPPC_CROSS) $(CPP_FLAGS) -o $@ -c $<

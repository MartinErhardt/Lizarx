SRCS_DLD = 	lib_stat/st_string.c \
	lib_stat/st_stdlib.c \
	lib_stat/video.c \
	ld.c
OBJS_DLD = $(addsuffix .o,$(basename $(SRCS_DLD)))
all: ld.a clean
ld.a: $(OBJS_DLD)
	$(AR_CROSS) rcs libdld.a $(OBJS_DLD)
%.o: %.c
	$(CC_CROSS) -o $@ -c $< -nostdinc
clean: 
	rm  $(OBJS_DLD) 

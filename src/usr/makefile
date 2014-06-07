SRCS_DLD = 	ld.a/lib_stat/st_string.c \
	ld.a/lib_stat/st_stdlib.c \
	ld.a/lib_stat/video.c \
	ld.a/ld.c
OBJS_DLD = $(addsuffix .o,$(basename $(SRCS_DLD)))

all: ld.a user clean

user:
	$(MAKE) -C tst1
	$(MAKE) -C tst2
	$(MAKE) -C tst3
	$(MAKE) -C tst4

ld.a: $(OBJS_DLD)
	$(AR_CROSS) rcs ld.a/libdld.a $(OBJS_DLD)
%.o: %.c
	$(CC_CROSS) -o $@ -c $< -nostdinc
clean: 
	rm  $(OBJS_DLD)

.PHONY: all ld.a tst1 tst2 clean

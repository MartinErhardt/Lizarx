all: kernel user

kernel:
	$(MAKE) -C kernel

user:
	$(MAKE) -C usr/tst1
	$(MAKE) -C usr/tst2
	
.PHONY: all kernel usr/tst1 usr/tst2
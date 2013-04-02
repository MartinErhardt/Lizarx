all: kernel user

kernel:
	$(MAKE) -C kernel

user:
	$(MAKE) -C usr/tst1
	$(MAKE) -C usr/tst2
	$(MAKE) -C usr/libC
	
.PHONY: all kernel usr/tst1 usr/tst2 usr/tst3
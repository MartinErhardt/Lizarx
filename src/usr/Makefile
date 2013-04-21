all: buildtools user

buildtools:
	./build_tools.sh

user:
	$(MAKE) -C tst1
	$(MAKE) -C tst2
	$(MAKE) -C tst3

.PHONY: all buildtools tst1 tst2 tst3

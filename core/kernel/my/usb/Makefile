inlinux:
	make -s library compiler=gcc
	make -s main compiler=gcc
	make -s binary linker=ld
crosscompile:
	make -s library compiler=x86_64-elf-gcc
	make -s main compiler=x86_64-elf-gcc
	make -s binary linker=x86_64-elf-ld
library:
	$(compiler) -c -fPIC -O2 lib/start.c
	$(compiler) -c -fPIC -O2 lib/print.c
main:
	$(compiler) -c -fPIC -O2 usb.c
	$(compiler) -c -fPIC -O2 hid.c
binary:
	$(linker) -T link/link.ld \
	start.o print.o \
	usb.o hid.o \
	-o usb.bin
clean:
	rm -f *.o
	rm -f *.bin

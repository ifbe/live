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
	nasm -f elf64 lib/interrupt.s -o interrupt.o
main:
	$(compiler) -c -fPIC -O2 ehci.c
	$(compiler) -c -fPIC -O2 xhci.c
	$(compiler) -c -fPIC -O2 isr.c
binary:
	$(linker) -T link/link.ld \
	start.o print.o interrupt.o \
	xhci.o ehci.o isr.o \
	-o xhci.bin
clean:
	rm -f *.o
	rm -f *.bin

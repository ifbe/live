inlinux:
	make -s library
	make -s main
	make -s binary
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	nasm -f elf64 lib/interrupt.s -o interrupt.o
main:
	gcc -c -fPIC -O2 xhci.c
	gcc -c -fPIC -O2 isr.c
binary:
	ld -T link/link.ld \
	start.o print.o interrupt.o \
	xhci.o isr.o \
	-o xhci.bin
clean:
	rm -f *.o
	rm -f *.bin

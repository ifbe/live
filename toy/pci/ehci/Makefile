binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o print.o interrupt.o \
	ehci.o isr.o \
	-o temp.bin
	nasm link/size.s -o ehci.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	nasm -f elf64 lib/interrupt.s -o interrupt.o
main:
	gcc -c -fPIC -O2 ehci.c
	gcc -c -fPIC -O2 isr.c
clean:
	rm -f *.o
	rm -f *.bin

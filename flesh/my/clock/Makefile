inlinux:
	make -s library
	make -s main
	make -s binary
library:
	nasm -f elf64 lib/start.s -o start.o
main:
	gcc -c -O2 clock.c
binary:
	ld -T link/link.ld \
	start.o \
	clock.o \
	-o clock.bin
clean:
	rm -f *.o
	rm -f *.bin

binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o \
	clock.o \
	-o clock.bin
library:
	nasm -f elf64 lib/start.s -o start.o
main:
	gcc -c -O2 clock.c
clean:
	rm -f *.o
	rm -f *.bin

inlinux:
	make -s library
	make -s main
	make -s binary
library:
	gcc -c -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 waiting.c
binary:
	ld -T link/link.ld \
	start.o hlt.o \
	waiting.o -o win8.bin
clean:
	rm -f *.o
	rm -f *.bin

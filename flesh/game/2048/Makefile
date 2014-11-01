inlinux:
	make -s library
	make -s main
	make -s binary
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -fPIC -O2 2048.c
binary:
	ld -T link/link.ld \
	start.o print.o hlt.o \
	2048.o \
	-o 2048.bin
clean:
	rm -f *.o
	rm -f *.bin

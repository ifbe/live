binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	2048.o \
	-o 2048
library:
	gcc -c -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 2048.c
clean:
	rm -f *.o 2048

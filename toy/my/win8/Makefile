binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	waiting.o -o win8.bin
library:
	gcc -c -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 waiting.c
clean:
	rm -f *.o
	rm -f *.bin

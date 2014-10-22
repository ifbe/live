binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	btree.o \
	-o btree.bin
library:
	gcc -c lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 btree.c
clean:
	rm -f *.o
	rm -f *.bin

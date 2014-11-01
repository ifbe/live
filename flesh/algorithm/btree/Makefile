inlinux:
	make -s library
	make -s main
	make -s binary
library:
	nasm -f elf64 lib/hlt.s -o hlt.o
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/convert.c
main:
	gcc -c -fPIC -O2 btree.c
binary:
	ld -T link/link.ld \
	start.o print.o convert.o hlt.o \
	btree.o \
	-o btree.bin
clean:
	rm -f *.o
	rm -f *.bin

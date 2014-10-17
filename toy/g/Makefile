binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	g.o \
	-o g
library:
	gcc -c -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 g.c
clean:
	rm -f *.o g

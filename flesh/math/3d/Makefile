binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	3d.o \
	-o 3d.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -fPIC -O2 3d.c
clean:
	rm -f *.o
	rm -f *.bin

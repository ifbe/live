inlinux:
	make -s library
	make -s agnesi
	make -s binary
library:
	gcc -c -fPIC -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
agnesi:
	gcc -c -fPIC -O2 agnesi.c
binary:
	ld -T link/link.ld \
	start.o hlt.o \
	agnesi.o \
	-o agnesi.bin
clean:
	rm -f *.o
	rm -f *.bin

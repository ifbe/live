binary:
	make -s library
	make -s agnesi
	ld -T link/link.ld \
	start.o hlt.o \
	agnesi.o \
	-o agnesi.bin
library:
	gcc -c -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
agnesi:
	gcc -c -O2 agnesi.c
clean:
	rm -f *.o
	rm -f *.bin

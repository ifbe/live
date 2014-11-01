binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o print.o hlt.o \
	xiangqi.o \
	-o xiangqi.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -fPIC -O2 xiangqi.c
clean:
	rm -f *.o
	rm -f *.bin

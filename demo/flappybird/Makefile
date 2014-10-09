binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	flappybird.o \
	-o bird
library:
	gcc -c -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 flappybird.c
clean:
	rm -f *.o bird

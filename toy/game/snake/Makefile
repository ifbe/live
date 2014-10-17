binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	snake.o \
	-o snake
library:
	gcc -c -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 snake.c
clean:
	rm -f *.o snake

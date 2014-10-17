binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	math.o \
	-o math
library:
	gcc -c lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 math.c
clean:
	rm -f *.o math

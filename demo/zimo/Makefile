binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	zimo.o \
	-o zimo
library:
	gcc -c -O2 lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 zimo.c
clean:
	rm -f *.o zimo

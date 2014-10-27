binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o print.o convert.o hlt.o \
	calc.o \
	-o calc.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/convert.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -fPIC -O2 calc.c
clean:
	rm -f *.o
	rm -f *.bin

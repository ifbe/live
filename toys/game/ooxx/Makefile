binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o print.o math.o hlt.o \
	ooxx.o \
	-o ooxx.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/math.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -fPIC -O2 ooxx.c
clean:
	rm -f *.o
	rm -f *.bin

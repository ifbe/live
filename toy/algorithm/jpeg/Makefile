binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o print.o malloc.o \
	jpeg.o decode.o \
	-o jpeg.bin
library:
	nasm -f elf64 lib/start.s -o start.o
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/malloc.c
main:
	gcc -c -fPIC -O2 jpeg.c
	gcc -c -fPIC -O2 decode.c
clean:
	rm -f *.o
	rm -f *.bin

binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o hlt.o \
	sketchpad.o \
	-o sketchpad
library:
	gcc -c lib/start.c
	nasm -f elf64 lib/hlt.s -o hlt.o
main:
	gcc -c -O2 sketchpad.c
clean:
	rm -f *.o sketchpad

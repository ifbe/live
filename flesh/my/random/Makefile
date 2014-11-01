inlinux:
	make -s library
	make -s main
	make -s binary
library:
	gcc -c -O2 lib/start.c
main:
	gcc -c -O2 random.c
binary:
	ld -T link/link.ld \
	start.o \
	random.o \
	-o random.bin
clean:
	rm -f *.o
	rm -f *.bin

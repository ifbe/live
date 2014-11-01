inlinux:
	make -s library
	make -s main
	make -s binary
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
main:
	gcc -c -fPIC -O2 ball2.c
binary:
	ld -T link/link.ld \
	start.o print.o \
	ball2.o \
	-o ball2.bin
clean:
	rm -f *.o
	rm -f *.bin

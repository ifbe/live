inlinux:
	make -s library
	make -s main
	make -s binary
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/usb.c
main:
	gcc -c -fPIC -O2 pl2303.c
binary:
	ld -T link/link.ld \
	start.o print.o usb.o \
	pl2303.o \
	-o pl2303.bin
clean:
	rm -f *.o
	rm -f *.bin

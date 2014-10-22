binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o print.o \
	usb.o hid.o \
	-o temp.bin
	nasm link/size.s -o usb.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
main:
	gcc -c -fPIC -O2 usb.c
	gcc -c -fPIC -O2 hid.c
clean:
	rm -f *.o
	rm -f *.bin

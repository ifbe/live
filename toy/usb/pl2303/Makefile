binary:
	make -s library
	make -s c
	ld -T link/link.ld \
	start.o print.o usb.o \
	pl2303.o \
	-o temp.bin
	nasm link/size.s -o pl2303.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/usb.c
c:
	gcc -c -fPIC -O2 pl2303.c
clean:
	rm -f *.o
	rm -f *.bin

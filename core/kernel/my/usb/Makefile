inlinux:
	make -s compilelibraryinlinux
	make -s compilemaininlinux
	ld -T link/link.ld \
	start.o print.o \
	usb.o hid.o \
	-o usb.bin
inwindows:
	make -s compilelibraryinwindows
	make -s compilemaininwindows
	ld -T link/link.ld \
	start.o print.o \
	usb.o hid.o \
	-o usb.bin
compilelibraryinlinux:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
compilelibraryinwindows:
	gcc -c -O2 lib/start.c
	gcc -c -O2 lib/print.c
compilemaininlinux:
	gcc -c -fPIC -O2 usb.c
	gcc -c -fPIC -O2 hid.c
compilemaininwindows:
	gcc -c -O2 usb.c
	gcc -c -O2 hid.c
clean:
	rm -f *.o
	rm -f *.bin

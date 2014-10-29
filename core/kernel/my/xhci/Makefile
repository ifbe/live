inlinux:
	make -s compilelibraryinlinux
	make -s compilemaininlinux
	ld -T link/link.ld \
	start.o print.o interrupt.o \
	xhci.o ehci.o isr.o \
	-o xhci.bin
inwindows:
	make -s compilelibraryinwindows
	make -s compilemaininwindows
	ld -T link/link.ld \
	start.o print.o interrupt.o \
	xhci.o ehci.o isr.o \
	-o xhci.bin
compilelibraryinlinux:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	nasm -f elf64 lib/interrupt.s -o interrupt.o
compilelibraryinwindows:
	gcc -c -O2 lib/start.c
	gcc -c -O2 lib/print.c
	nasm -f win64 lib/interrupt.s -o interrupt.o
compilemaininlinux:
	gcc -c -fPIC -O2 ehci.c
	gcc -c -fPIC -O2 xhci.c
	gcc -c -fPIC -O2 isr.c
compilemaininwindows:
	gcc -c -O2 ehci.c
	gcc -c -O2 xhci.c
	gcc -c -O2 isr.c
clean:
	rm -f *.o
	rm -f *.bin

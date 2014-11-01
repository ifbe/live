inlinux:
	make -s library
	make -s c
	make -s binary
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
c:
	gcc -c -fPIC -O2 hello.c
binary:
	ld -T link/link.ld \
	start.o print.o \
	hello.o \
	-o hello.bin
clean:
	rm -f *.o
	rm -f *.bin

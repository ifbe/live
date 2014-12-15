inlinux:
	make -s library
	make -s main
	make -s binary
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/math.c
main:
	gcc -c -fPIC -O2 circle.c
binary:
	ld -T link/link.ld \
	start.o print.o math.o \
	circle.o \
	-o circle.bin
clean:
	rm -f *.o
	rm -f *.bin

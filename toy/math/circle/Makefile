binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o \
	circle.o \
	-o circle.bin
library:
	gcc -c lib/start.c
main:
	gcc -c -O2 circle.c
clean:
	rm -f *.o
	rm -f *.bin

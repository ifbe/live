binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o \
	ball2.o \
	-o ball2.bin
library:
	gcc -c lib/start.c
main:
	gcc -c -O2 ball2.c
clean:
	rm -f *.o
	rm -f *.bin

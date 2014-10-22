binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o \
	ball.o \
	-o ball.bin
library:
	gcc -c lib/start.c
main:
	gcc -c -O2 ball.c
clean:
	rm -f *.o
	rm -f *.bin

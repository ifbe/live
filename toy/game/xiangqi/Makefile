binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o \
	xiangqi.o \
	-o xiangqi.bin
library:
	gcc -c -O2 lib/start.c
main:
	gcc -c -O2 xiangqi.c
clean:
	rm -f *.o
	rm -f *.bin

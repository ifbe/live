inlinux:
	make -s library
	make -s color
	make -s binary
library:
	gcc -c -O2 lib/start.c
color:
	gcc -c -O2 color.c
binary:
	ld -T link/link.ld \
	start.o \
	color.o \
	-o color.bin
clean:
	rm -f *.o
	rm -f *.bin

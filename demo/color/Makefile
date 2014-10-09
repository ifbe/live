binary:
	make -s library
	make -s color
	ld -T link/link.ld \
	start.o \
	color.o \
	-o color
library:
	gcc -c -O2 lib/start.c
color:
	gcc -c -O2 color.c
clean:
	rm -f *.o color

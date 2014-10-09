binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o \
	mandelbrot.o \
	-o mandelbrot
library:
	gcc -c lib/start.c
main:
	gcc -c -O2 mandelbrot.c
clean:
	rm -f *.o mandelbrot

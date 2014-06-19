mandelbrot:
	gcc -c start.c
	gcc -c -O2 mandelbrot.c
	ld -T link.ld start.o mandelbrot.o -o mandelbrot
clean:
	rm -f *.o mandelbrot

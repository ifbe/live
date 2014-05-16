c:
	gcc -c start.c
	gcc -c -O2 3d.c
	ld -T link.ld start.o 3d.o -o ../demo
clean:
	rm -f *.o

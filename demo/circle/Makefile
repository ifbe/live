c:
	gcc -c start.c
	gcc -c -O2 circle.c
	ld -T link.ld start.o circle.o -o ../demo
clean:
	rm -f *.o

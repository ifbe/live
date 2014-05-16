c:
	gcc -c start.c
	gcc -c -O2 math.c
	ld -T link.ld start.o math.o -o ../demo
clean:
	rm -f *.o

multitask:
	gcc -c -O2 start.c
	gcc -c -O2 2048.c
	ld -T link.ld start.o 2048.o -o ../demo
clean:
	rm -f *.o temp*

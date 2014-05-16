multitask:
	gcc -c -O2 start.c
	gcc -c -O2 color.c
	ld -T link.ld start.o color.o -o ../demo
clean:
	rm -f *.o temp*

multitask:
	gcc -c -O2 start.c
	gcc -c -O2 zimo.c
	ld -T link.ld start.o zimo.o -o ../demo
clean:
	rm -f *.o temp*

multitask:
	gcc -c -O2 start.c
	gcc -c -O2 russia.c
	ld -T link.ld start.o russia.o -o ../demo
clean:
	rm -f *.o temp*

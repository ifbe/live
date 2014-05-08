multitask:
	gcc -c -O2 start.c
	gcc -c -O2 random.c
	ld -T link.ld start.o random.o -o ../demo
clean:
	rm -f *.o temp*

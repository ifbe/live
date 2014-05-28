random:
	gcc -c -O2 start.c
	gcc -c -O2 random.c
	ld -T link.ld start.o random.o -o random
clean:
	rm -f *.o random

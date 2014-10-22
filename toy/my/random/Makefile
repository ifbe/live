random:
	gcc -c -O2 start.c
	gcc -c -O2 random.c
	ld -T link.ld start.o random.o -o random.bin
clean:
	rm -f *.o
	rm -f *.bin

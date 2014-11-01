c:
	gcc -c start.c
	gcc -c -O2 low.c
	ld -T link.ld start.o low.o -o out.bin
clean:
	rm -f *.o
	rm -f *.bin

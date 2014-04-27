c:
	rm -f temp*
	gcc -c -fPIC -O2 start.c -o start.o
	ld -T link.ld start.o -o temp.bin
	nasm end.s -o temp
clean:
	rm -f *.o
	rm -f temp*

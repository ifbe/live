c:
	gcc -c -fPIC -O2 start.c
	gcc -c -fPIC -O2 print.c
	gcc -c -fPIC -O2 xhci.c
	ld -T link.ld start.o print.o xhci.o -o temp.bin
	nasm end.s -o temp
clean:
	rm -f *.o
	rm -f temp*

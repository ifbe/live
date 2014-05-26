c:
	nasm -f elf64 start.s -o start.o
	gcc -c -fPIC -O2 xhci.c
	gcc -c -fPIC -O2 print.c
	ld -T link.ld start.o xhci.o print.o -o temp.bin
	nasm end.s -o temp
clean:
	rm -f *.o
	rm -f temp*

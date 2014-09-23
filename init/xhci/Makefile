c:
	gcc -c -fPIC -O2 start.c
	gcc -c -fPIC -O2 ehci.c
	gcc -c -fPIC -O2 xhci.c
	gcc -c -fPIC -O2 print.c
	nasm -f elf64 interrupt.s
	ld -T link.ld start.o ehci.o xhci.o print.o interrupt.o -o temp.bin
	nasm size.s -o temp
clean:
	rm -f *.o
	rm -f temp*

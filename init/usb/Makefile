c:
	gcc -c -fPIC -O2 start.c
	gcc -c -fPIC -O2 usb.c
	gcc -c -fPIC -O2 print.c
	nasm -f elf64 interrupt.s
	ld -T link.ld start.o usb.o interrupt.o print.o -o temp.bin
	nasm size.s -o temp
clean:
	rm -f *.o
	rm -f temp*

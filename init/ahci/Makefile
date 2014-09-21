c:
	gcc -c -fPIC -O2 start.c
	gcc -c -fPIC -O2 print.c
	gcc -c -fPIC -O2 ahci.c
	ld -T link.ld start.o print.o ahci.o -o temp.bin
	nasm size.s -o temp
clean:
	rm -f *.o
	rm -f temp*

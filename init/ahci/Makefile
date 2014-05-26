c:
	nasm -f elf64 start.s -o start.o
	gcc -c -fPIC -O2 print.c -o print.o
	gcc -c -fPIC -O2 ahci.c -o ahci.o
	gcc -c -fPIC -O2 id.c -o id.o
	gcc -c -fPIC -O2 read.c -o read.o
	gcc -c -fPIC -O2 disk.c -o disk.o
	gcc -c -fPIC -O2 fs.c -o fs.o
	ld -T link.ld start.o print.o ahci.o id.o read.o disk.o fs.o -o temp.bin
	nasm end.s -o temp
clean:
	rm -f *.o
	rm -f temp*

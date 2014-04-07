c:
	rm -f temp*
	gcc -c -fPIC -O2 start.c -o start.o
	gcc -c -O2 print.c -o print.o
	gcc -c -fPIC -O2 initahci.c -o initahci.o
	gcc -c -fPIC -O2 identify.c -o identify.o
	gcc -c -fPIC -O2 read.c -o read.o
	gcc -c -fPIC -O2 disk.c -o disk.o
	gcc -c -fPIC -O2 fs.c -o fs.o
	nasm -f elf64 gccbug.s -o gccbug.o
	ld -T link.ld -o temp \
	start.o print.o initahci.o identify.o read.o disk.o fs.o gccbug.o
clean:
	rm -f *.o
	rm -f temp

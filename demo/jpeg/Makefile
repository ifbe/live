c:
	nasm -f elf64 start.s -o temp.o
	gcc -c jpeg.c -o temp1.o
	ld -T link.ld temp.o temp1.o -o ../demo
clean:
	rm -f temp temp*

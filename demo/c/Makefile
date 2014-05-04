c:
	nasm -f elf64 start.s -o temp.o
	gcc -c -fPIC -O2 1024x768/jpeg.c -o temp1.o
	ld -T link.ld temp.o temp1.o -o ../demo
clean:
	rm -f temp*

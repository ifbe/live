2048:
	gcc -c -O2 start.c
	gcc -c -O2 2048.c
	nasm -f elf64 fuckgcc.s
	ld -T link.ld start.o 2048.o fuckgcc.o -o 2048
clean:
	rm -f *.o 2048

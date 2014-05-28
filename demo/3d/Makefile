c:
	gcc -c start.c
	gcc -c -O2 3d.c
	nasm -f elf64 fuckgcc.s
	ld -T link.ld start.o 3d.o fuckgcc.o -o 3d
clean:
	rm -f *.o 3d

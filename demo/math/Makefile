math:
	gcc -c start.c
	gcc -c -O2 math.c
	nasm -f elf64 fuckgcc.s
	ld -T link.ld start.o math.o fuckgcc.o -o math
clean:
	rm -f *.o math

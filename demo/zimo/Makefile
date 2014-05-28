zimo:
	gcc -c -O2 start.c
	gcc -c -O2 zimo.c
	nasm -f elf64 fuckgcc.s
	ld -T link.ld start.o zimo.o fuckgcc.o -o zimo
clean:
	rm -f *.o zimo

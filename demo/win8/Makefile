multitask:
	gcc -c -O2 start.c
	gcc -c -O2 waiting.c
	nasm -f elf64 fuckgcc.s
	ld -T link.ld start.o waiting.o fuckgcc.o -o ../demo
clean:
	rm -f *.o temp*

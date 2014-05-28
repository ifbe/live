bird:
	gcc -c -O2 start.c
	gcc -c -O2 flappybird.c
	nasm -f elf64 fuckgcc.s
	ld -T link.ld start.o flappybird.o fuckgcc.o -o bird
clean:
	rm -f *.o bird

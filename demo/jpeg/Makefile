jpeg:
	nasm -f elf64 start.s
	gcc -c jpeg.c
	ld -T link.ld start.o jpeg.o -o jpeg
clean:
	rm -f *.o jpeg

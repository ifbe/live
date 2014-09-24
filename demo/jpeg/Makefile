jpeg:
	nasm -f elf64 start.s
	gcc -c -fPIC -O2 jpeg.c
	gcc -c -fPIC -O2 print.c
	ld -T link.ld start.o jpeg.o print.o -o jpeg
clean:
	rm -f *.o jpeg

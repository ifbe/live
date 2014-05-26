apictimer:
	nasm -f elf64 start.s
	gcc -c -O2 clock.c
	ld -T link.ld start.o clock.o -o ../demo
clean:
	rm -f *.o

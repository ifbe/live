sketchpad:
	gcc -c start.c
	gcc -c -O2 sketchpad.c
	nasm -f elf64 fuckgcc.s
	ld -T link.ld start.o sketchpad.o fuckgcc.o -o math
clean:
	rm -f *.o sketchpad

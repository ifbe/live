jarvis:
	nasm jarvis.s -o ../kernel
	gcc -Wall -c jpeg.c -o ../temp.elf
	ld -T 100000.ld ../temp.elf -o ../temp

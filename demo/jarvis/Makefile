jarvis:
	nasm jarvis.s -o temp
	gcc -Wall -c jpeg.c -o temp.o
	ld -T link.ld temp.o -o temp1
	nasm link.s -o ../demo
clean:
	rm -f temp*

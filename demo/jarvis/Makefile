jarvis:
	nasm jarvis.s -o temp
	gcc -c -fPIC jpeg.c -o temp.o
	ld -T link.ld temp.o -o temp1
	nasm link.s -o jarvis
clean:
	rm -f temp*
	rm -f jarvis

c:
	gcc -c -fPIC start.c -o temp.o
	gcc -c -fPIC -O2 1024x768/print.c -o temp1.o
	ld -T link.ld temp.o temp1.o -o ../demo
clean:
	rm -f temp*

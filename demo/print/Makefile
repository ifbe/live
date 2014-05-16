c:
	gcc -c start.c
	gcc -c -O2 print.c
	ld -T link.ld start.o print.o -o ../demo
clean:
	rm -f *.o

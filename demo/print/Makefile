print:
	gcc -c start.c
	gcc -c -O2 print.c
	ld -T link.ld start.o print.o -o print
clean:
	rm -f *.o print

port:
	gcc -c start.c
	gcc -c -O2 port.c
	ld -T link.ld start.o port.o -o port
clean:
	rm -f *.o port

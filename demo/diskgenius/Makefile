c:
	gcc -c start.c -o temp.o
	gcc -c -O2 diskgenius.c -o temp1.o
	gcc -c -O2 identify.c -o temp2.o
	gcc -c -O2 read.c -o temp3.o
	ld -T link.ld temp.o temp1.o temp2.o temp3.o -o ../demo
clean:
	rm -f temp*

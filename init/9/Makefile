c:
	gcc -c start.c -o temp.o
	gcc -c -O2 ahci.c -o temp1.o
	gcc -c -O2 print.c -o temp2.o
	gcc -c -O2 identify.c -o temp3.o
	gcc -c -O2 read.c -o temp4.o
	gcc -c -O2 diskgenius.c -o temp5.o
	ld -T link.ld temp.o temp1.o temp2.o temp3.o temp4.o temp5.o -o temp
clean:
	rm -f temp*

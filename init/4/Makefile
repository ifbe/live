c:
	rm -f temp*
	gcc -c -fPIC -O2 start.c -o temp.o
	gcc -c -O2 print.c -o temp1.o
	gcc -c -fPIC -O2 initahci.c -o temp2.o
	gcc -c -fPIC -O2 identify.c -o temp3.o
	gcc -c -fPIC -O2 read.c -o temp4.o
	gcc -c -fPIC -O2 disk.c -o temp5.o
	gcc -c -fPIC -O2 filesystem.c -o temp6.o
	ld -T link.ld -o temp \
	temp.o temp1.o temp2.o temp3.o temp4.o temp5.o temp6.o
clean:
	rm -f temp*

c:
	gcc -c start.c -o temp.o
	gcc -c -O2 1024x768/taiji.c -o temp1.o
	ld -T link.ld temp.o temp1.o -o ../demo
clean:
	rm -f temp*

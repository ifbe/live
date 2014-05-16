c:
	gcc -c start.c
	gcc -c -O2 taiji.c
	ld -T link.ld start.o taiji.o -o ../demo
clean:
	rm -f *.o

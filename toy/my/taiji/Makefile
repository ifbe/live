taiji:
	gcc -c start.c
	gcc -c -O2 taiji.c
	ld -T link.ld start.o taiji.o -o taiji.bin
clean:
	rm -f *.o
	rm -f *.bin

ball:
	gcc -c start.c
	gcc -c -O2 ball.c
	ld -T link.ld start.o ball.o -o ball
clean:
	rm -f *.o ball

multitask:
	gcc -c -fPIC -O2 start.c
	gcc -c -fPIC -O2 taiji.c
	gcc -c -fPIC -O2 ball.c
	ld -T 1.ld start.o taiji.o -o temp1
	ld -T 2.ld start.o ball.o -o temp2
	nasm kernel.s -o multi
clean:
	rm -f *.o temp1 temp2 multi

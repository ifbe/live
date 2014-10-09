binary:
	make -s library
	make -s main
	ld -T link/1.ld start.o taiji.o -o temp1
	ld -T link/2.ld start.o ball.o -o temp2
	nasm lib/kernel.s -o multi
library:
	gcc -c -fPIC -O2 lib/start.c
main:
	gcc -c -fPIC -O2 taiji.c
	gcc -c -fPIC -O2 ball.c
clean:
	rm -f *.o temp1 temp2 multi

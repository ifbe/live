inlinux:
	make -s library
	make -s main
	make -s binary
library:
	gcc -c -fPIC -O2 lib/start.c
main:
	gcc -c -fPIC -O2 taiji.c
	gcc -c -fPIC -O2 ball.c
binary:
	ld -T link.ld start.o taiji.o -o temp1.bin
	ld -T link.ld start.o ball.o -o temp2.bin
	nasm lib/kernel.s -o multi.bin
clean:
	rm -f *.o
	rm -f *.bin

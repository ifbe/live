multitask:
	gcc -Wall -c -O2 taiji.c -o temp1.o
	objdump -d temp1.o | grep -m 1 start
	ld -T 200000.ld temp1.o -o temp1
	gcc -Wall -c -O2 ball.c -o temp2.o
	objdump -d temp1.o | grep -m 1 start
	ld -T 100000.ld temp2.o -o temp2
	nasm kernel.s -o temp
	nasm link.s -o ../demo
clean:
	rm -f temp*

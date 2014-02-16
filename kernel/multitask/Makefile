multi:
	nasm kernel.s -o ../kernel
	gcc -Wall -c -O2 taiji.c -o ../temp.elf
	objdump -d ../temp.elf | grep -m 1 start
	ld -T 200000.ld ../temp.elf -o ../temp
	gcc -Wall -c -O2 ball.c -o ../temp1.elf
	objdump -d ../temp1.elf | grep -m 1 start
	ld -T 100000.ld ../temp1.elf -o ../temp1

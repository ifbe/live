c:
	nasm wrap.s -o ../kernel
	gcc -O2 -c ball.c -o ../temp.elf
	echo -n "change link.s:"
	objdump -d ../temp.elf | grep -m 1 start
	ld -T link.ld ../temp.elf -o ../temp

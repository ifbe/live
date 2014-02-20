c:
	nasm caller.s -o ../kernel
	gcc -O2 -c math.c -o ../temp.elf
	echo -n "change caller.s use this:"
	objdump -d ../temp.elf | grep -m 1 start
	ld -T 100000.ld ../temp.elf -o ../temp

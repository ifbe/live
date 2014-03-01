c:
	#kernel
	nasm call.s -o ../kernel
	#c
	gcc -O2 -c jpeg.c -o ../temp.elf
	#black magic
	objdump -d ../temp.elf | grep -m 1 start
	objdump -d ../temp.elf | grep -m 1 start>>../kernel
	#temp
	ld -T 100000.ld ../temp.elf -o ../temp

c:
	#kernel
	nasm call.s -o ../demo
	#c
	gcc -c -O2 identify.c -o ../temp.elf
	#black magic
	objdump -d ../temp.elf | grep -m 1 start
	objdump -d ../temp.elf | grep -m 1 start>>../demo
	#temp
	ld -T 100000.ld ../temp.elf -o ../temp

c:
	#kernel
	nasm call.s -o ../demo
	#c
	gcc -c -O2 diskid.c -o ../temp.elf
	#black magic
	objdump -d ../temp.elf | grep -m 1 start
	objdump -d ../temp.elf | grep -m 1 start>>../demo
	#temp
	ld -T link.ld ../temp.elf -o ../temp

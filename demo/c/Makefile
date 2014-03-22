c:
	#1
	nasm call.s -o ../demo
	#2
	gcc -c -O2 readdisk.c -o temp.elf
	ld -T link.ld temp.elf -o temp
	#black magic
	objdump -d temp.elf | grep -m 1 start
	objdump -d temp.elf | grep -m 1 start>>../demo
	#1+2
	nasm link.s
clean:
	rm -f temp*

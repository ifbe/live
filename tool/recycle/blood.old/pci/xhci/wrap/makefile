linux:
	gcc -c -fPIC -O2 start.c
	gcc -c -fPIC -O2 print.c
	nasm -f elf64 interrupt.s -o interrupt.o
cross:
	x86_64-elf-gcc -c -fPIC -O2 start.c
	x86_64-elf-gcc -c -fPIC -O2 print.c
	nasm -f elf64 interrupt.s -o interrupt.o

btree:
	gcc -c start.c
	gcc -c -O2 btree.c
	nasm -f elf64 fuckgcc.s -o fuckgcc.o
	ld -T link.ld start.o btree.o fuckgcc.o -o btree
clean:
	rm -f *.o btree

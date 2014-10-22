image:
	make -s clean
	make -s -C cpu
	make -s -C kernel/my
	nasm -s init.s
clean:
	make clean -s -C cpu
	make clean -s -C kernel/my
	rm -f init

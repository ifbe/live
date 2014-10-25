image:
	make -s clean
	make -s -C cpu/flat64
	make -s -C kernel/my
	nasm -s init.s -o init.bin
clean:
	make clean -s -C cpu/flat64
	make clean -s -C kernel/my
	rm -f *.bin

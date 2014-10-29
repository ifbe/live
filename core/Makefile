inlinux:
	make -s clean
	make -s -C cpu/flat64
	make -s -C kernel/my inlinux
	nasm -s init.s -o init.bin
inwindows:
	make -s clean
	make -s -C cpu/flat64
	make -s -C kernel/my inwindows
	nasm -s init.s -o init.bin
inmac:
	make -s clean
	make -s -C cpu/flat64
	make -s -C kernel/my inmac
	nasm -s init.s -o init.bin
clean:
	make -s -C cpu/flat64 clean
	make -s -C kernel/my clean
	rm -f *.bin

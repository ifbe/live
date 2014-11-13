inlinux:
	make -s clean
	make -s -C cpu/flat64
	make -s -C kernel/my inlinux
	nasm -s core.s -o core.bin
crosscompile:
	make -s clean
	make -s -C cpu/flat64
	make -s -C kernel/my crosscompile
	nasm -s core.s -o core.bin
clean:
	make -s -C cpu/flat64 clean
	make -s -C kernel/my clean
	rm -f *.bin

all:
	make -s -C 0
	make -s -C 1000
	make -s -C 2000
	make -s -C 4000
	nasm core.s -o ../core.bin
clean:
	make -s -C 4000 clean
	rm -f *.bin

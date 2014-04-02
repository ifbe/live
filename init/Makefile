init:
	make -C ahci
	make -C fakekernel
	nasm init.s
clean:
	make clean -C ahci
	make clean -C fakekernel
	rm -f init

fakekernel:
	make -C ahci
	nasm jarvis.s -o temp
clean:
	make clean -s -C ahci
	rm -f temp

binary:
	make -s -C 0000-mbr
	make -s -C 0200-realmode
	make -s -C 4000-longmode
	yasm asm.s -o asm.bin
clean:
	rm -f *.o *.img

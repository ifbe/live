binary:
	make -s -C 0.mbr
	make -s -C 1.asm
	make -s -C 2.cxx
	nasm head.s -o ../head.img
cross:
	make -s -C 0.mbr
	make -s -C 1.asm
	make -s -C 2.cxx cross
	nasm head.s -o ../head.img
clean:
	rm -f *.o *.img

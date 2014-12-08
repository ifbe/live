binary:
	make -s -C wrap linux
	make -s -C main linux
	ld -T link.ld \
	wrap/start.o wrap/print.o \
	main/ahci.o \
	-o ahci.bin
cross:
	make -s -C wrap cross
	make -s -C main cross
	x86_64-elf-ld -T link.ld \
	wrap/start.o wrap/print.o \
	main/ahci.o \
	-o ahci.bin
clean:
	rm -f */*.o *.bin

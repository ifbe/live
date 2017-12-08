link := ld
binary:
	make -s -C libboot0 linux
	make -s -C libboot1 linux
	make -s -C libhard0 linux
	make -s -C libhard1 linux
	make -s -C libsoft0 linux
	make -s -C libsoft1 linux
	make -s -C libui0 linux
	make -s -C libui1 linux
	make -s link linker=ld
cross:
	make -s -C libboot0 cross
	make -s -C libboot1 cross
	make -s -C libhard0 cross
	make -s -C libhard1 cross
	make -s -C libsoft0 cross
	make -s -C libsoft1 cross
	make -s -C libui0 cross
	make -s -C libui1 cross
	make -s link linker=x86_64-elf-ld
link:
	$(linker) -T link.ld \
	libboot0/start.o \
	libboot0/basic.o \
	libboot1/*.o \
	libhard0/*.o \
	libhard1/*.o \
	libsoft0/*.o \
	libsoft1/*.o \
	libui0/*.o \
	libui1/*.o \
	-o cxx.bin
clean:
	rm -f */*.o *.bin

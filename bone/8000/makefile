binary:
	make -s -C init linux
	make -s -C libhard linux
	make -s -C libsoft linux
	make -s -C libsoft1 linux
	make -s -C main linux

	ld -T link.ld \
	init/start.o init/ide.o init/ahci.o init/xhcihost.o init/xhciport.o \
	main/main.o \
	libsoft1/parttable.o libsoft1/fat.o libsoft1/ntfs.o libsoft1/ext.o \
	libsoft/convert.o libsoft/remember.o \
	libhard/journal.o libhard/identify.o libhard/read.o \
	-o extremities.bin
cross:
	make -s -C init cross
	make -s -C libhard cross
	make -s -C libsoft cross
	make -s -C libsoft1 cross
	make -s -C main cross

	x86_64-elf-ld -T link.ld \
	init/start.o init/ide.o init/ahci.o init/xhcihost.o init/xhciport.o \
	main/main.o \
	libsoft1/parttable.o libsoft1/fat.o libsoft1/ntfs.o libsoft1/ext.o \
	libsoft/convert.o libsoft/remember.o \
	libhard/journal.o libhard/identify.o libhard/read.o \
	-o extremities.bin
clean:
	rm -f */*.o *.bin

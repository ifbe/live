binary:
	make -s -C init linux
	make -s -C wrap0 linux
	make -s -C wrap1 linux
	make -s -C wrap2 linux
	ld -T link.ld \
	init/start.o init/ahci.o \
	wrap2/master.o wrap2/fat.o wrap2/ntfs.o wrap2/ext.o \
	wrap1/print.o wrap1/remember.o wrap1/else.o \
	wrap0/identify.o wrap0/read.o \
	-o ../extremities.bin
cross:
	make -s -C init cross
	make -s -C wrap0 cross
	make -s -C wrap1 cross
	make -s -C wrap2 cross
	x86_64-elf-ld -T link.ld \
	init/start.o init/ahci.o \
	wrap2/master.o wrap2/fat.o wrap2/ntfs.o wrap2/ext.o \
	wrap1/print.o wrap1/remember.o wrap1/else.o \
	wrap0/identify.o wrap0/read.o \
	-o ../extremities.bin
clean:
	rm -f */*.o *.bin

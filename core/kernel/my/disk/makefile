binary:
	make -s -C wrap linux
	make -s -C main linux
	ld -T link.ld \
	wrap/start.o wrap/print.o wrap/remember.o wrap/else.o \
	wrap/identify.o wrap/read.o \
	main/master.o main/fat.o main/ntfs.o main/ext.o \
	-o disk.bin
cross:
	make -s -C wrap cross
	make -s -C main cross
	x86_64-elf-ld -T link.ld \
	wrap/start.o wrap/print.o wrap/remember.o wrap/else.o \
	wrap/identify.o wrap/read.o \
	main/master.o main/fat.o main/ntfs.o main/ext.o \
	-o disk.bin
clean:
	rm -f */*.o *.bin

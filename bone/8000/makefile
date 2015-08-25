binary:
	make -s -C init linux
	make -s -C wrap linux
	make -s -C wrap1 linux
	make -s -C wrap2 linux
	make -s -C main linux
	ld -T link.ld init/start.o \
	init/ide.o init/ahci.o init/xhci.o init/usb.o \
	main/master.o \
	wrap2/fat.o wrap2/ntfs.o wrap2/ext.o \
	wrap1/remember.o wrap1/else.o \
	wrap/journal.o wrap/identify.o wrap/read.o \
	-o extremities.bin
cross:
	make -s -C init cross
	make -s -C wrap cross
	make -s -C wrap1 cross
	make -s -C wrap2 cross
	make -s -C main cross
	x86_64-elf-ld -T link.ld init/start.o \
	init/ide.o init/ahci.o init/xhci.o init/usb.o \
	main/master.o \
	wrap2/fat.o wrap2/ntfs.o wrap2/ext.o \
	wrap1/remember.o wrap1/else.o \
	wrap/journal.o wrap/identify.o wrap/read.o \
	-o extremities.bin
clean:
	rm -f */*.o *.bin

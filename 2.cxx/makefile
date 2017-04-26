link := ld
binary:
	make -s -C init linux
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
	make -s -C init cross
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
	init/start.o init/main.o \
	libboot0/print.o \
	libboot1/debug.o \
	libhard0/fp.o \
	libhard0/port.o \
	libhard0/ide.o \
	libhard0/ahci.o \
	libhard0/xhcihost.o \
	libhard0/xhciport.o \
	libhard0/ps2kbd.o \
	libhard1/kbd.o \
	libhard1/identify.o \
	libhard1/read.o \
	libsoft1/acpi.o \
	libsoft1/convert.o \
	libsoft1/parttable.o \
	libsoft1/fs.fat.o \
	libsoft1/fs.ntfs.o \
	libsoft1/fs.ext.o \
	libui0/screen.o \
	libui1/ascii.o \
	libui1/character.o \
	-o cxx.bin
clean:
	rm -f */*.o *.bin

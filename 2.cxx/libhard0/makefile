cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) cpu/fp.c
	$(compiler) $(cflag) cpu/port.c
	$(compiler) $(cflag) input/ps2kbd.c
	$(compiler) $(cflag) storage/ahci.c
	$(compiler) $(cflag) storage/ide.c
	$(compiler) $(cflag) transfer/pci/pci.c
	$(compiler) $(cflag) transfer/usb/xhcihost.c
	$(compiler) $(cflag) transfer/usb/xhciport.c
clean:
	rm -f *.o

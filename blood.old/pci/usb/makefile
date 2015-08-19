binary:
	make -s -C wrap linux
	make -s -C main linux
	ld -T link.ld \
	wrap/start.o wrap/print.o \
	main/usb.o main/hid.o \
	-o usb.bin
cross:
	make -s -C wrap cross
	make -s -C main cross
	x86_64-elf-ld -T link.ld \
	wrap/start.o wrap/print.o \
	main/usb.o main/hid.o \
	-o usb.bin
clean:
	rm -f */*.o *.bin

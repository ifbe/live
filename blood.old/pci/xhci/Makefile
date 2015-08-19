binary:
	make -s -C wrap linux
	make -s -C main linux
	ld -T link.ld \
	wrap/start.o wrap/print.o wrap/interrupt.o \
	main/xhci.o main/ehci.o main/isr.o \
	-o xhci.bin
cross:
	make -s -C wrap cross
	make -s -C main cross
	x86_64-elf-ld -T link.ld \
	wrap/start.o wrap/print.o wrap/interrupt.o \
	main/xhci.o main/ehci.o main/isr.o \
	-o xhci.bin
clean:
	rm -f */*.o *.bin

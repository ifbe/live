c:
	gcc -c -fPIC -O2 start.c
	gcc -c -fPIC -O2 ethernet.c
	gcc -c -fPIC -O2 print.c
	gcc -c -fPIC -O2 remember.c
	nasm -f elf64 interrupt.s
	ld -T link.ld \
	start.o ethernet.o print.o remember.o interrupt.o \
	-o temp.bin
	nasm size.s -o ethernet
test:
	make -s c
	make -C ../.. mountfat
	sudo cp ethernet /mnt/nbd/live
	make -C ../.. umountfat
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-net nic \
	-net tap,ifname=tap0,script=no \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/fat.vhd

clean:
	rm -f *.o
	rm -f temp*
	rm -f ethernet

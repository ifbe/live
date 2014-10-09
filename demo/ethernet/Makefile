binary:
	make -s library
	make -s c
	ld -T link/link.ld \
	start.o print.o remember.o interrupt.o \
	ethernet.o  \
	-o temp.bin
	nasm link/size.s -o ethernet
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/remember.c
	nasm -f elf64 lib/interrupt.s -o interrupt.o
c:
	gcc -c -fPIC -O2 ethernet.c
test:
	make -s binary
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

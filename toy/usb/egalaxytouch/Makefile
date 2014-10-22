binary:
	make -s library
	make -s c
	ld -T link/link.ld \
	start.o print.o usb.o \
	egalaxytouch.o \
	-o temp.bin
	nasm link/size.s -o touch.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/usb.c
c:
	gcc -c -fPIC -O2 egalaxytouch.c
#插上egalaxytouch,lsusb,根据结果改hostbus=，hostaddr=这一段
test:
	make -s binary
	make -C ../../.. mountfat
	sudo cp touch.bin /mnt/nbd/live/touch
	make -C ../../.. umountfat
	lsusb
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-net nic \
	-net tap,ifname=tap0,script=no \
	-device nec-usb-xhci,id=xhci \
        -device usb-host,hostbus=3,hostaddr=3,bus=xhci.0,port=3 \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/fat.vhd
clean:
	rm -f *.o
	rm -f *.bin

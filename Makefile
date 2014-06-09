image:
	make -s clean
	make -s -C load
	make -s -C init
	nasm else/link.s -o live
clean:
	make clean -s -C load
	make clean -s -C init
	rm -f live
fat:
	make image
	sudo modprobe nbd max_part=4
	sudo qemu-nbd -c /dev/nbd0 /mnt/fuck/image/v/fat.vhd
	sudo partprobe /dev/nbd0
	sudo mount -o rw /dev/nbd0p1 /mnt/nbd
	sudo cp live /mnt/nbd/live/live
	sleep 2s
	sudo umount /dev/nbd0p1
	sudo qemu-nbd -d /dev/nbd0
	sudo rmmod nbd
ntfs:
	make image
	sudo modprobe nbd max_part=4
	sudo qemu-nbd -c /dev/nbd0 /mnt/fuck/image/v/ntfs.vhd
	sudo partprobe /dev/nbd0
	sudo mount -o rw /dev/nbd0p1 /mnt/nbd
	sudo cp live /mnt/nbd/live/live
	sudo umount /dev/nbd0p1
	sudo qemu-nbd -d /dev/nbd0
	sudo rmmod nbd
ext:
	make image
	sudo modprobe nbd max_part=4
	sudo qemu-nbd -c /dev/nbd0 /mnt/fuck/image/v/ext.vhd
	sudo partprobe /dev/nbd0
	sudo mount -o rw /dev/nbd0p1 /mnt/nbd
	sudo cp live /mnt/nbd/live/live
	sudo umount /dev/nbd0p1
	sudo qemu-nbd -d /dev/nbd0
	sudo rmmod nbd
qemutest:
	qemu-kvm \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=live
usbtest:
	sudo qemu-kvm \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/dev/sdb
fattest:
	sudo qemu-kvm \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/fat.vhd
ntfstest:
	sudo qemu-kvm \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/ntfs.vhd
exttest:
	sudo qemu-kvm \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/ext.vhd
push:
	make clean
	make clean -s -C demo
	git add --all .
	git commit -a
	git push

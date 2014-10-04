image:
	make -s clean
	make -s -C load
	make -s -C init
	nasm else/link.s -o live
clean:
	make clean -s -C load
	make clean -s -C init
	rm -f live
mountfat:
	sudo modprobe nbd max_part=4
	sudo qemu-nbd -c /dev/nbd0 /mnt/fuck/image/v/fat.vhd
	sudo partprobe /dev/nbd0
	sudo mount -o rw /dev/nbd0p1 /mnt/nbd
umountfat:
	sudo umount /dev/nbd0p1
	sudo qemu-nbd -d /dev/nbd0
	sudo rmmod nbd
fat:
	make -s image
	make -s mountfat
	sudo cp live /mnt/nbd/live/live
	make -s umountfat
mountntfs:
	sudo modprobe nbd max_part=4
	sudo qemu-nbd -c /dev/nbd0 /mnt/fuck/image/v/ntfs.vhd
	sudo partprobe /dev/nbd0
	sudo mount -o rw /dev/nbd0p1 /mnt/nbd
umountntfs:
	sudo umount /dev/nbd0p1
	sudo qemu-nbd -d /dev/nbd0
	sudo rmmod nbd
ntfs:
	make -s image
	make -s mountntfs
	sudo cp live /mnt/nbd/live/live
	make -s umountntfs
mountext:
	sudo modprobe nbd max_part=4
	sudo qemu-nbd -c /dev/nbd0 /mnt/fuck/image/v/ext.vhd
	sudo partprobe /dev/nbd0
	sudo mount -o rw /dev/nbd0p1 /mnt/nbd
umountext:
	sudo umount /dev/nbd0p1
	sudo qemu-nbd -d /dev/nbd0
	sudo rmmod nbd
ext:
	make -s image
	make -s mountext
	sudo cp init/init /mnt/nbd/live/init
	make -s umountext
fattest:
	sudo qemu-kvm \
	-smp 2 \
	-m 512 \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/fat.vhd
ntfstest:
	sudo qemu-kvm \
	-smp 2 \
	-m 512 \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/ntfs.vhd
exttest:
	sudo qemu-kvm \
	-smp 2 \
	-m 512 \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/ext.vhd
flashdrivetest:
	sudo qemu-kvm \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device usb-tablet,bus=xhci.0,port=1 \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/dev/sdb
qemutest:
	make -s image
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device usb-tablet,bus=xhci.0,port=1 \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=live
#插上鼠标键盘，lsusb，根据结果改"hostbus=?,hostaddr=?"这一段
hidtest:
	make -s image
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device usb-host,hostbus=2,hostaddr=6,bus=xhci.0,port=3 \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=live
#插上pl2303，lsusb，根据结果改"hostbus=?,hostaddr=?"这一段
pl2303test:
	make -s image
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device usb-host,hostbus=2,hostaddr=3,bus=xhci.0,port=3 \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=live
#插上u盘，lsusb，根据结果改"hostbus=?,hostaddr=?"这一段
massstoragetest:
	make -s image
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device usb-host,hostbus=2,hostaddr=4,bus=xhci.0,port=3 \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=live
copy:
	make -s image
	sudo cp init/init /mnt/efi/live/init
everything:
	make -s fat
	make -s ntfs
	make -s ext
	make -s copy
push:
	make -s clean
	make clean -s -C demo
	git add --all .
	git commit -a
	git push

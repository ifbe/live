image:
	make -s -C loader
	make -s -C core
	nasm script/img.s -o live
clean:
	make clean -s -C loader
	make clean -s -C core
	rm -f live
#本Makefile生成的自己引导img虚拟硬盘
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
#准备一个引导已经做好的fat.vhd虚拟硬盘,自己改位置和挂载点
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
fattest:
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/fat.vhd
#准备一个引导已经做好的ntfs.vhd虚拟硬盘，自己改位置和挂载点
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
ntfstest:
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/ntfs.vhd
#准备一个引导已经做好的ext.vhd虚拟硬盘，自己改位置和挂载点
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
	sudo cp core/init /mnt/nbd/live/init
	make -s umountext
exttest:
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/ext.vhd
#准备一个引导已经做好的bootmgr.vhd虚拟硬盘，自己改位置和挂载点
mountbootmgr:
	sudo modprobe nbd max_part=4
	sudo qemu-nbd -c /dev/nbd0 /mnt/fuck/image/v/bootmgr.vhd
	sudo partprobe /dev/nbd0
	sudo mount -o rw /dev/nbd0p1 /mnt/nbd
umountbootmgr:
	sudo umount /dev/nbd0p1
	sudo qemu-nbd -d /dev/nbd0
	sudo rmmod nbd
bootmgr:
	make -s image
	make -s mountbootmgr
	sudo cp core/init /mnt/nbd/live/init
	make -s umountbootmgr
bootmgrtest:
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/mnt/fuck/image/v/bootmgr.vhd
#准备一个引导已经做好的u盘，用qemu测试u盘启动，自己改/dev/sd?
flashdrivetest:
	sudo qemu-kvm \
	-monitor stdio \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device usb-tablet,bus=xhci.0,port=1 \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=/dev/sdb
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
#下面是我自己的方便干活脚本，别管了
copy:
	make -s image
	sudo cp core/init /mnt/efi/live/init
everything:
	make -s image
	make -s fat
	make -s ntfs
	make -s ext
	make -s copy
push:
	make -s clean
	git add --all .
	git commit -a
	git push

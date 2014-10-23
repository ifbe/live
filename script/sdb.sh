#插上的U盘应该是/dev/sdb，当然也有可能是sdc。。。自己改一下就行
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

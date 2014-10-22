#插上U盘，应该是sdb，不是的话自己改一下
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

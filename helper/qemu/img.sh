#本Makefile生成的自己引导img虚拟硬盘
sudo qemu-kvm \
-monitor stdio \
-smp 2 \
-m 512 \
-device usb-ehci,id=ehci \
-device nec-usb-xhci,id=xhci \
-device usb-tablet,bus=xhci.0,port=1 \
-device ahci,id=ahci \
-device ide-drive,drive=disk,bus=ahci.0 \
-drive id=disk,if=none,file=live.img

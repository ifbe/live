if [ x$1 == x ]
then
        echo "错误:请指定磁盘镜像的绝对地址(也可以是/dev/sdb这种)"
        echo "正确用法:./qemu.sh /where/name.format"
        exit -1
fi


sudo qemu-kvm \
-monitor stdio \
-smp 2 \
-m 512 \
-device ahci,id=ahci \
-device ide-drive,drive=disk,bus=ahci.0 \
-drive id=disk,if=none,file=$1 \
-device usb-ehci,id=ehci \
-device nec-usb-xhci,id=xhci \
-device usb-tablet,bus=xhci.0,port=1 \
-device usb-hub,id=hub1,bus=xhci.0,port=2 \
-device usb-host,vendorid=0x0eef,productid=0x1,bus=xhci.0,port=3 \
-device usb-host,vendorid=0x067b,productid=0x2303,bus=xhci.0,port=4 \
-device usb-net,bus=xhci.0,port=2.1

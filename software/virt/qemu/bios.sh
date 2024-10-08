#!/bin/sh
#1.usb-kbd will overwide ps2kbd
#2.usb-mouse or usb-tablet will overwide ps2kbd

dir=`dirname $0`
cd $dir

#-d int,cpu_reset \
#-no-shutdown \
#-device nec-usb-xhci,id=xhci \
#-device usb-hub,id=hub1,bus=xhci.0,port=2 \
#-device usb-net,bus=xhci.0,port=3 \
#-device usb-kbd,bus=xhci.0,port=1 \
#-device usb-mouse,bus=xhci.0,port=3 \
$1 \
-no-reboot \
-serial stdio \
-smp 2 \
-m 512 \
-drive id=diskvhd,if=none,format=vpc,file=$3 \
-drive id=diskimg,if=none,format=raw,file=$2 \
-device ahci,id=ahci \
-device ide-hd,drive=diskimg,bus=ahci.0 \
-device usb-ehci,id=ehci \
-device qemu-xhci,id=xhci \
-device usb-tablet,bus=xhci.0,port=1 \
-device usb-storage,drive=diskvhd,bus=xhci.0,port=4

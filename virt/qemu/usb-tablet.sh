#!/bin/sh
dir=`dirname $0`
cd $dir

#-d int,cpu_reset \
#-no-shutdown \
#-device usb-audio,bus=xhci.0,port=1 \
#-device usb-hub,id=hub1,bus=xhci.0,port=4
$1 \
-no-reboot \
-bios ovmf.fd \
-serial stdio \
-smp 2 \
-m 512 \
-drive id=diskimg,if=none,format=raw,file=$2 \
-drive id=diskvhd,if=none,format=vpc,file=$3 \
-device ahci,id=ahci \
-device ide-hd,drive=diskimg,bus=ahci.0 \
-device usb-ehci,id=ehci \
-device qemu-xhci,id=xhci \
-device usb-kbd,bus=xhci.0,port=1 \
-device usb-tablet,bus=xhci.0,port=2 \
-device usb-net,bus=xhci.0,port=3 \
-device usb-storage,drive=diskvhd,bus=xhci.0,port=4

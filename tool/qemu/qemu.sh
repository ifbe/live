#!/bin/sh
dir=`dirname $0`
cd $dir

$1 \
-monitor stdio \
-smp 2 \
-m 512 \
-device ahci,id=ahci \
-device ide-drive,drive=disk,bus=ahci.0 \
-drive id=disk,if=none,format=vpc,file=$2 \
-device usb-ehci,id=ehci \
-device nec-usb-xhci,id=xhci \
-device usb-tablet,bus=xhci.0,port=1 \
-device usb-hub,id=hub1,bus=xhci.0,port=2 \
-device usb-net,bus=xhci.0,port=2.1
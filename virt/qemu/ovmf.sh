#!/bin/sh
dir=`dirname $0`
cd $dir

$1 \
-d cpu_reset \
-no-reboot \
-no-shutdown \
-bios ovmf.fd \
-serial stdio \
-smp 2 \
-m 512 \
$2

#!/bin/sh
dir=`dirname $0`
cd $dir

$1 \
-bios ovmf.fd \
-serial stdio \
-smp 2 \
-m 512 \
$2

#!/bin/sh
dir=`dirname $0`
cd $dir

$1 -bios ovmf.fd $2

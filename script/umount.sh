sudo umount /dev/nbd0p1
sudo qemu-nbd -d /dev/nbd0
sudo rmmod nbd

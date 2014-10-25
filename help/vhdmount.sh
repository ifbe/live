if [ x$1 == x ]
then
        echo "错误:请指定磁盘镜像的绝对地址"
        echo "正确用法:./mount.sh /where/name.format"
        exit -1
fi


sudo modprobe nbd max_part=4
sudo qemu-nbd -c /dev/nbd0 $1
sudo partprobe /dev/nbd0
sudo mount -o rw /dev/nbd0p1 /mnt/nbd

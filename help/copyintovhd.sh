#把文件拷进vhd第1个分区的live文件夹里面


if [ x$1 == x ]||[ x$2 == x ]
then
        echo "错误:请指定文件的绝对地址和磁盘镜像的绝对地址"
        echo "正确用法:./copy.sh /where/filename /where/diskimage.format"
        exit -1
fi

sudo modprobe nbd max_part=4
sudo qemu-nbd -c /dev/nbd0 $2
sudo partprobe /dev/nbd0
sudo mount -o rw /dev/nbd0p1 /mnt/nbd

sudo cp $1 /mnt/nbd/live/

sudo umount /dev/nbd0p1
sudo qemu-nbd -d /dev/nbd0
sudo rmmod nbd

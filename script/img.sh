if [ x$1 == x ]
then
        echo "错误:请指定我这个源在你硬盘上的绝对地址"
        echo "正确用法:./img.sh /home/ifbe/live"
        exit -1
fi


make -s -C $1/loader
make -s -C $1/core

#[0,64m]
dd if=/dev/zero of=$1/live.img bs=512 count=131072
#mbr
dd if=$1/loader/load.bin of=$1/live.img conv=notrunc
#[0x10000,128k)
dd if=$1/core/init.bin of=$1/live.img bs=512 seek=128 conv=notrunc

sudo losetup /dev/loop0 $1/live.img
sudo partprobe /dev/loop0
sudo mkfs.vfat /dev/loop0p1
sudo losetup -d /dev/loop0

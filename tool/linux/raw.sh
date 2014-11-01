if [ x$1 == x ]
then
        echo "错误:请指定我这个源在你硬盘上的绝对地址"
        echo "正确用法:./img.sh /home/ifbe/live"
        exit -1
fi


#[0,64m]
dd if=/dev/zero of=$1/live.img bs=512 count=131072
#[0,64k)
dd if=$1/core/init.bin of=$1/live.img conv=notrunc

sudo losetup /dev/loop0 $1/live.img			#loop设备
sudo partprobe /dev/loop0
sudo mkfs.vfat /dev/loop0p1				#fat32文件系统
sudo partprobe /dev/loop0
sudo mkdir -p /mnt/tempmountpointforlive		#临时挂载点
sudo mount /dev/loop0p1 /mnt/tempmountpointforlive	#挂载fat32
sudo mkdir -p /mnt/tempmountpointforlive/live		#在fat32里面建一些东西
sudo touch /mnt/tempmountpointforlive/live/something
sudo umount /mnt/tempmountpointforlive			#卸载fat32
sudo losetup -d /dev/loop0				#卸载loop设备

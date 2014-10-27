if [ x$1 == x ]
then
        echo "错误:请指定镜像在你硬盘上的绝对地址"
        echo "正确用法:./img.sh /where/name.img"
        exit -1
fi


sudo losetup /dev/loop0 $1				#loop设备
sudo partprobe /dev/loop0
sudo mkdir -p /mnt/tempmountpointforlive		#临时挂载点
sudo mount /dev/loop0p1 /mnt/tempmountpointforlive	#挂载fat32

if [ x$1 == x ]||[ x$2 == x ]
then
        echo "错误:请指定文件的绝对地址和磁盘镜像的绝对地址"
        echo "正确用法:./copy.sh /where/filename /where/diskimage.format"
        exit -1
fi



sudo losetup /dev/loop0 $2				#loop设备
sudo partprobe /dev/loop0
sudo mkdir -p /mnt/tempmountpointforlive		#临时挂载点
sudo mount /dev/loop0p1 /mnt/tempmountpointforlive	#挂载fat32

sudo cp $1 /mnt/tempmountpointforlive/live/

sudo umount /mnt/tempmountpointforlive			#卸载fat32
sudo losetup -d /dev/loop0				#卸载loop设备

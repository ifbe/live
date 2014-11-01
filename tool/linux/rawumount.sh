if [ x$1 == x ]
then
        echo "错误:请指定我这个源在你硬盘上的绝对地址"
        echo "正确用法:./img.sh /home/ifbe/live"
        exit -1
fi


sudo umount /mnt/tempmountpointforlive			#卸载fat32
sudo losetup -d /dev/loop0				#卸载loop设备

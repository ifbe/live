root = $(shell pwd)


#toy:


#我写好了一个脚本来制作raw格式磁盘镜像
#总共64m，只有一个分区在[1m,64m)，已经格式化为fat32
img:
	make -s -C core
	/bin/sh help/raw.sh $(root)


#script/qemu.sh指定了cpu数量，内存数量，透传进去的设备vidpid，磁盘等东西
#qemu的qemu.sh脚本相当于vmware的.vmx配置文件
test:
	/bin/sh help/qemu.sh $(root)/live.img


clean:
	make clean -s -C core
	rm -f *.img


push:
	make clean
	git add --all .
	git commit -a
	git push

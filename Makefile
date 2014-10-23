root = $(shell pwd)


#toy:


img:
	/bin/sh script/img.sh $(root)


#qemu.sh指定了cpu数量，内存数量，透传进去的设备vidpid，磁盘等东西
#想换配置改一下script/qemu.sh这个文件即可
test:
	/bin/sh script/qemu.sh $(root)/live.img


clean:
	make clean -s -C loader
	make clean -s -C core
	rm -f *.img


push:
	make clean
	git add --all .
	git commit -a
	git push

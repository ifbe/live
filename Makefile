#这个Makefile就是用来制作处理测试磁盘镜像的
#如果只要那个裸奔的程序，可以除了core文件夹以外其他的全部删掉
root = $(shell pwd)
toy =


inlinuxcreateraw:
	make -s -C core inlinux
	/bin/sh help/raw.sh $(root)
inlinuxcreatevhd:
	make -s -C core inlinux
	/bin/sh help/vhd.sh $(root)
inwindowscreateraw:
	make -s -C core inwindows
	/bin/sh help/raw.bat $(root)
inwindowscreatevhd:
	make -s -C core inwindows
	/bin/sh help/vhd.bat $(root)


testimg:
	/bin/sh help/qemu.sh $(root)/live.img


testtoy:			#比如make testtoy toy=`pwd`/toy/game/2048
ifneq ($(toy),)
	make -s -C $(toy)
	/bin/sh help/copyintoraw.sh $(toy)/*.bin $(root)/live.img
	make -s -C $(toy) clean
	make -s testimg
endif


clean:
	make clean -s -C core
	rm -f *.img


push:
	make clean
	git add --all .
	git commit -a
	git push

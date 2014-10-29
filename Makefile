#这个Makefile就是用来制作处理测试磁盘镜像的
#如果只要那个裸奔的程序，可以除了core文件夹以外其他的全部删掉
root = $(shell pwd)
toydir =


createrawinlinux:
	make -s -C core inlinux
	/bin/sh help/raw.sh $(root)
createvhdinlinux:
	make -s -C core inlinux
	/bin/sh help/vhd.sh $(root)
createrawinwindows:
	make -s -C core inwindows
	/bin/sh help/raw.bat $(root)
createvhdinwindows:
	make -s -C core inwindows
	/bin/sh help/vhd.bat $(root)


test:
	/bin/sh help/qemu.sh $(root)/live.img


toy:			#比如make toy toydir=`pwd`/toy/game/2048
ifneq ($(toydir),)
	make -s -C $(toydir)
	/bin/sh help/copyintoraw.sh $(toydir)/*.bin $(root)/live.img
	make -s -C $(toydir) clean
	make -s testimg
endif


clean:
	make clean -s -C core
	rm -f *.img


push:
	make -s clean
	git add --all .
	git commit -a
	git push

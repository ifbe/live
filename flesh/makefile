#0:在core里做出.bin，在这里输入比如这样的make toy=algorithm/jpeg
toy = algorithm/jpeg/jpeg.bin
toydir = $(shell dirname $(toy))


#1:把../core/core.bin与刚make出来的??.bin合成成一个img文件
img:
	make -s -C ../core
	make -s -C $(toydir)
	make -s generateimg
	echo -n "test" | dd of=test.img bs=1 seek=448 conv=notrunc
crosscompile:
	make -s -C ../core crosscompile
	make -s -C $(toydir) crosscompile
	make -s generateimg
	echo "test" | dd of=test.img bs=1 seek=448 conv=notrunc


#2:
generateimg:
	echo -e "incbin \"../core/core.bin\"\ntimes 0x10000-(\$$-\$$\$$)db 0\nincbin \"$(toy)\"\ntimes 0x100000-(\$$-\$$\$$)db 0" > compose.s
	nasm compose.s -o test.img
	rm compose.s


#3:
kvm:
	../tool/kvm $(shell pwd)/test.img
qemu:
	../tool/qemu $(shell pwd)/test.img
vmware:
	cp test.img temp.img
	dd if=temp.img of=test.img bs=512 seek=2048 conv=notrunc
	cp test.img temp.img
	dd if=temp.img of=test.img bs=512 seek=4096 conv=notrunc
	qemu-img convert -f raw -O vpc test.img test.vhd
	cp test.vhd ../tool/vmware/test.vhd
	#(please double click) ../tool/vmware/vmware.vmx


#...
clean:
	rm -f *.img *.vhd

binary:
	make -s -C cpu/flat64
	make -s -C kernel/my binary
	nasm -s other/64k.s -o core.bin
cross:
	make -s -C cpu/flat64
	make -s -C kernel/my cross
	nasm -s other/64k.s -o core.bin
bin2img:
	nasm -s other/64m.s -o core.img
img2dmg:
	qemu-img convert -f raw -O dmg core.img core.dmg
img2vhd:
	qemu-img convert -f raw -O vpc core.img core.vhd
img2vmdk:
	qemu-img convert -f raw -O vmdk core.img core.vmdk
qemu:
	other/qemu $(shell pwd)/core.img
clean:
	make -s -C cpu/flat64 clean
	make -s -C kernel/my clean
	rm -f *.bin *.img *.vmdk *.vhd *.dmg *.vdi

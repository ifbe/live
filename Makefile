default:
	cat README
purecode:
	make -s clean
	sh else/makecode
a:
	make -s clean
	make -s -C demo/assembly
	make -s image
c:
	make -s clean
	make -s -C demo/c
	make -s image
d:
	make -s clean
	make -s -C demo/diskgenius
	make -s image
e:
	make -s clean
	make -s -C demo/easiest
	make -s image
j:
	make -s clean
	make -s -C demo/jarvis
	make -s image
m:
	make -s clean
	make -s -C demo/multitask
	make -s image
clean:
	rm -f load/load
	rm -f init/init
	rm -f init/\[c000\,ffff\]/temp*
	rm -f demo/demo
	rm -f demo/temp*
	rm -f live
	rm -rf purecode
qemu:
	qemu-kvm \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=live
image:
	make -s -C load
	make -s -C init
	touch load/load
	touch init/init
	touch demo/temp
	touch demo/temp1
	nasm else/link.s -o live
	make qemu
push:
	make clean
	git add --all .
	git commit -a
	git push

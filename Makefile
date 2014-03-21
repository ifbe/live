default:
	cat README
a:
	make -s clean
	make -s -C load
	make -s -C init
	make -s -C demo/assembly
	make -s image
	make -s qemu
c:
	make -s clean
	make -s -C load
	make -s -C init
	make -s -C demo/c
	make -s image
	make -s qemu
j:
	make -s clean
	make -s -C load
	make -s -C init
	make -s -C demo/jarvis
	make -s image
	make -s qemu
m:
	make -s clean
	make -s -C load
	make -s -C init
	make -s -C demo/multitask
	make -s image
	make -s qemu
purecode:
	sh else/makecode
image:
	touch load/load
	touch init/init
	touch demo/kernel
	touch demo/temp
	touch demo/temp1
	nasm else/link.s -o live
clean:
	rm -f load/load
	rm -f init/init
	rm -f init/\[e000\,efff\]/temp*
	rm -f init/\[f000\,ffff\]/temp*
	rm -f demo/kernel
	rm -f demo/temp*
	rm -f live
	rm -rf purecode
push:
	make clean
	git add --all .
	git commit -a
	git push
qemu:
	qemu-kvm \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=live

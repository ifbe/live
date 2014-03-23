image:
	make -s clean
	make -s -C load
	make -s -C init
	make -s -C demo
	nasm else/link.s -o live
	make -s qemu
purecode:
	make -s clean
	sh else/makecode
clean:
	make clean -s -C load
	make clean -s -C init
	make clean -s -C demo
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
push:
	make clean
	git add --all .
	git commit -a
	git push

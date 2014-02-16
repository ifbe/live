j:
	make clean
	make -C load
	make -C init
	make -C kernel/jarvis
	make image
	make test
m:
	make clean
	make -C load
	make -C init
	make -C kernel/multitask
	make image
	make test
a:
	make clean
	make -C load
	make -C init
	make -C kernel/assembly
	make image
	make test
c:
	make clean
	make -C load
	make -C init
	make -C kernel/c
	make image
	make test
image:
	touch load/load
	touch init/init
	touch kernel/kernel
	touch kernel/temp
	touch kernel/temp1
	nasm link.s -o live
clean:
	rm -f load/load
	rm -f init/init
	rm -f kernel/kernel
	rm -f kernel/temp*
	rm -f live
push:
	git add --all .
	git commit -a
	git push
test:
	qemu-kvm \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=live

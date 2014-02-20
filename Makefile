a:
	make -s clean
	make -s -C load
	make -s -C init
	make -s -C kernel/assembly
	make -s image
	make -s test
c:
	make -s clean
	make -s -C load
	make -s -C init
	make -s -C kernel/c
	make -s image
	make -s test
j:
	make -s clean
	make -s -C load
	make -s -C init
	make -s -C kernel/jarvis
	make -s image
	make -s test
m:
	make -s clean
	make -s -C load
	make -s -C init
	make -s -C kernel/multitask
	make -s image
	make -s test
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
	make clean
	git add --all .
	git commit -a
	git push
test:
	qemu-kvm \
	-smp 2 \
	-m 512 \
	-device usb-ehci,id=ehci \
	-device nec-usb-xhci,id=xhci \
	-device ahci,id=ahci \
	-device ide-drive,drive=disk,bus=ahci.0 \
	-drive id=disk,if=none,file=live

#----------------step1: head.img----------------
flag := binary
mbr:
	make -C arch/x64/bios $(flag)
	mv arch/x64/bios/head.img .
efi:
	make -C arch/x64/efi
	mv arch/x64/efi/BOOTX64.EFI .




#----------------step2: body.img----------------
linuxmkfs:
	dd if=/dev/zero of=body.img bs=1048576 count=62
	mkfs.fat -F32 body.img
linuxmount:
	sudo losetup /dev/loop0 live.img
	sudo partprobe /dev/loop0
linuxumount:
	sudo losetup -d /dev/loop0

macmkfs:
	dd if=/dev/zero of=body.img bs=1048576 count=62
	make -s macerase haha=$(shell hdiutil attach -nomount body.img)
macerase:
	diskutil eraseVolume FAT32 BODY ${haha}
	hdiutil detach ${haha}
macmount:
	hdiutil attach -nomount body.img
macumount:
	hdiutil detach body.img

winmkfs:
	fsutil file createnew body.img 65011712
winmount:
	echo ???
winumount:
	echo ???




#----------------step3: live.img----------------
biosimg:
	cp head.img live.img
	dd if=body.img of=live.img bs=1048576 seek=1 conv=notrunc
	qemu-img resize -f raw live.img 64M
	qemu-img convert -f raw -O vmdk live.img live.vmdk
	qemu-img convert -f raw -O vpc -o subformat=fixed live.img live.vhd
efiimg:
	dd if=/dev/zero of=body.img bs=1k count=1440
	mformat -i body.img -f 1440 ::
	mmd -i body.img ::/EFI
	mmd -i body.img ::/EFI/BOOT
	mcopy -i body.img BOOTX64.EFI ::/EFI/BOOT
	make -s biosimg




#----------------step4: testing----------------
qemu:
	tool/qemu/qemu.sh "qemu-system-x86_64" $(shell pwd)/live.vhd
ovmf:
	tool/qemu/ovmf.sh "qemu-system-x86_64" $(shell pwd)/live.vhd
bochs:
	bochs -f tool/bochs/bochsrc
vmware:
	cp live.vhd tool/vmware/live.vhd
	#(please double click) ../tool/vmware/vmware.vmx
virtualbox:
parallels:




clean:
	rm -f *.o *.bin *.efi *.EFI
	rm -f *.bin *.efi *.img *.vmdk *.vhd *.dmg *.vdi

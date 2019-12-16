#----------------step0: asm----------------
mbr:
	make -C arch/x64/bios binary
	cp arch/x64/bios/asm.bin 00-asm.img
efi:
	make -C arch/x64/efi
	cp arch/x64/efi/BOOTX64.EFI .




#----------------step1: cxx----------------
easy:
	make -C core/easy
	cp core/easy/cxx.bin 01-cxx.img
easy-cross:
	make -C core/easy cross
	cp core/easy/cxx.bin 01-cxx.img
full:
	make -C core/full
	cp core/full/cxx.bin 01-cxx.img
full-cross:
	make -C core/full cross
	cp core/full/cxx.bin 01-cxx.img




#----------------step2: fat.img----------------
linuxmkfs:
	dd if=/dev/zero of=02-fat.img bs=1048576 count=62
	mkfs.fat -F32 02-fat.img
linuxmount:
	sudo losetup /dev/loop0 live.img
	sudo partprobe /dev/loop0
linuxumount:
	sudo losetup -d /dev/loop0

macmkfs:
	dd if=/dev/zero of=02-fat.img bs=1048576 count=62
	#make -s macerase haha=$(shell hdiutil attach -nomount 02-fat.img)
macerase:
	diskutil eraseVolume FAT32 BODY ${haha}
	hdiutil detach ${haha}
macmount:
	hdiutil attach -nomount 02-fat.img
macumount:
	hdiutil detach 02-fat.img

winmkfs:
	fsutil file createnew 02-fat.img 65011712
winmount:
	echo ???
winumount:
	echo ???




#----------------step3: live.img----------------
biosimg:
	dd if=00-asm.img of=live.img conv=notrunc
	dd if=01-cxx.img of=live.img bs=32768 seek=1 conv=notrunc
	dd if=02-fat.img of=live.img bs=1048576 seek=1 conv=notrunc
	qemu-img resize -f raw live.img 64M
	qemu-img convert -f raw -O vmdk live.img live.vmdk
	qemu-img convert -f raw -O vpc -o subformat=fixed live.img live.vhd
efiimg:
	dd if=/dev/zero of=02-fat.img bs=1k count=1440
	mformat -i 02-fat.img -f 1440 ::
	mmd -i 02-fat.img ::/EFI
	mmd -i 02-fat.img ::/EFI/BOOT
	mcopy -i 02-fat.img BOOTX64.EFI ::/EFI/BOOT
	make -s biosimg




#----------------step4: testing----------------
qemu:
	virt/qemu/qemu.sh "qemu-system-x86_64" $(shell pwd)/live.vhd
ovmf:
	virt/qemu/ovmf.sh "qemu-system-x86_64" $(shell pwd)/live.vhd
bochs:
	bochs -f virt/bochs/bochsrc
vmware:
	cp live.vhd virt/vmware/live.vhd
	#(please double click) ../virt/vmware/vmware.vmx
virtualbox:
parallels:




clean:
	rm -f *.o *.bin *.efi *.EFI
	rm -f *.bin *.efi *.img *.vmdk *.vhd *.dmg *.vdi

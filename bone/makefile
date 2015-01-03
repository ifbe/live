#1:create a 64k binary,compile with gcc(linux) or x86_64-elf-gcc(other system)
binary:				#linuxer only
	make -s -C cpu
	make -s -C jarvis		#[0x2000,0x3fff]
	make -s -C ahci binary		#[0x4000,0x5fff]
	make -s -C xhci binary		#[0x6000,0x7fff]
	make -s -C usb binary		#[0x8000,0xbfff]
	make -s -C disk binary		#[0xc000,0xffff]
	nasm -s 64k.s -o core.bin
crosscompile:		#windows and mac user
	make -s -C cpu
	make -s -C jarvis		#[0x2000,0x3fff]
	make -s -C ahci cross		#[0x4000,0x5fff]
	make -s -C xhci cross		#[0x6000,0x7fff]
	make -s -C usb cross		#[0x8000,0xbfff]
	make -s -C disk cross		#[0xc000,0xffff]
	nasm -s 64k.s -o core.bin


#2:create img
bin2img:
	nasm -s 64m.s -o core.img


#3:dmg or vhd or vmdk
img2dmg:
	qemu-img convert -f raw -O dmg core.img core.dmg
img2vhd:
	qemu-img convert -f raw -O vpc core.img core.vhd
img2vmdk:
	qemu-img convert -f raw -O vmdk core.img core.vmdk


#4:test with many software
kvm:
	../tool/kvm $(shell pwd)/core.img
qemu:
	../tool/qemu $(shell pwd)/core.img
bochs:
	bochs -f ../tool/bochsrc
vmware:
	cp core.vhd ../tool/vmware/test.vhd
	#(please double click) ../tool/vmware/vmware.vmx
virtualbox:
parallels:


#...
clean:
	make -s -C cpu clean
	make -s -C jarvis clean
	make -s -C ahci clean
	make -s -C xhci clean
	make -s -C usb clean
	make -s -C disk clean
	rm -f *.bin *.img *.vmdk *.vhd *.dmg *.vdi

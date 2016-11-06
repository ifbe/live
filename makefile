#1:create a 64k binary,compile with gcc(linux) or x86_64-elf-gcc(other system)
binary:				#linuxer only
	make -s -C 0.bios
	make -s -C 1.asm
	make -s -C 2.ccc
	nasm corebin.s -o core.bin
	qemu-img convert -f raw -O vpc core.bin core.vhd
cross:		#windows and mac user
	make -s -C 0.bios
	make -s -C 1.asm
	make -s -C 2.ccc cross
	nasm corebin.s -o core.bin
	qemu-img convert -f raw -O vpc core.bin core.vhd




#image convert
img2dmg:
	qemu-img convert -f raw -O dmg core.img core.dmg
img2vhd:
	qemu-img convert -f raw -O vpc core.img core.vhd
img2vmdk:
	qemu-img convert -f raw -O vmdk core.img core.vmdk




#test
kvm:
	tool/kvm $(shell pwd)/core.img
qemu:
	tool/qemu $(shell pwd)/core.img
bochs:
	bochs -f ../tool/bochsrc
vmware:
	cp core.vhd ../tool/vmware/test.vhd
	#(please double click) ../tool/vmware/vmware.vmx
virtualbox:
parallels:




clean:
	make -s -C 4000 clean
	rm -f */*.o */*.bin */*.efi
	rm -f *.bin *.efi *.img *.vmdk *.vhd *.dmg *.vdi

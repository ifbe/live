#1:create a 64k binary,compile with gcc(linux) or x86_64-elf-gcc(other system)
binary:				#linuxer only
	make -s -C 0.bios
	make -s -C 1.asm
	make -s -C 2.cxx
	make -s convert
cross:		#windows and mac user
	make -s -C 0.bios
	make -s -C 1.asm
	make -s -C 2.cxx cross
	make -s convert




#image convert
convert:
	nasm README -o core.bin
	cp core.bin core.img
	qemu-img resize -f raw core.img 1M
	qemu-img convert -f raw -O vmdk core.img core.vmdk
	qemu-img convert -f raw -O vpc -o subformat=fixed core.img core.vhd




#test
qemu:
	tool/qemu/qemu.sh "qemu-system-x86_64" \
	$(shell pwd)/core.vhd
ovmf:
	tool/qemu/qemu.sh "qemu-system-x86_64 -bios ovmf.fd" \
	$(shell pwd)/core.vhd
bochs:
	bochs -f ../tool/bochs/bochsrc
vmware:
	cp core.vhd ../tool/vmware/test.vhd
	#(please double click) ../tool/vmware/vmware.vmx
virtualbox:
parallels:




clean:
	make -s -C 2.cxx clean
	rm -f */*.o */*.bin */*.efi
	rm -f *.bin *.efi *.img *.vmdk *.vhd *.dmg *.vdi

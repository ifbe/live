#1:create a 64k binary,compile with gcc(linux) or x86_64-elf-gcc(other system)
binary:		#linuxer only
	make -s -C cpu/flat64
	make -s -C kernel/my binary
	nasm -s tool/64k.s -o core.bin
cross:		#windows and mac user
	make -s -C cpu/flat64
	make -s -C kernel/my cross
	nasm -s tool/64k.s -o core.bin


#2:create img
bin2img:
	nasm -s tool/64m.s -o core.img


#3:dmg or vhd or vmdk
img2dmg:
	qemu-img convert -f raw -O dmg core.img core.dmg
img2vhd:
	qemu-img convert -f raw -O vpc core.img core.vhd
img2vmdk:
	qemu-img convert -f raw -O vmdk core.img core.vmdk


#4:test with many software
kvm:
	tool/kvm $(shell pwd)/core.img
qemu:
	tool/qemu $(shell pwd)/core.img
bochs:
	bochs -f tool/bochsrc
vmware:
virtualbox:
parallels:


#...
clean:
	make -s -C cpu/flat64 clean
	make -s -C kernel/my clean
	rm -f *.bin *.img *.vmdk *.vhd *.dmg *.vdi

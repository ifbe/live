#1:create a 64k binary,compile with gcc(linux) or x86_64-elf-gcc(other system)
binary:				#linuxer only
	make -s -C cpu
	make -s -C kernel binary
	nasm -s 64k.s -o core.bin
crosscompile:		#windows and mac user
	make -s -C cpu
	make -s -C kernel cross
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
	make -s -C kernel clean
	rm -f *.bin *.img *.vmdk *.vhd *.dmg *.vdi

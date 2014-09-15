init:
	#[0,0x1fff]
	make -s -C cpu
	#[0x2000,0x3fff]
	make -s -C jarvis
	#[0x4000,0x5fff]
	make -s -C ahci
	#[0x6000,0x7fff]
	make -s -C xhci
	#[0x8000,0xbfff]
	make -s -C usb
	#[0xc000,0xffff]
	make -s -C disk
	#one cat to rule them all
	cat cpu/temp jarvis/temp ahci/temp xhci/temp usb/temp disk/temp > init
clean:
	make clean -s -C cpu
	make clean -s -C jarvis
	make clean -s -C ahci
	make clean -s -C xhci
	make clean -s -C usb
	make clean -s -C disk
	rm -f init

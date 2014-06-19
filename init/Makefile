init:
	#0x4000
	make -s -C cpu
	#0x4000
	make -s -C jarvis
	#0x1000
	make -s -C ahci
	#0x1000
	make -s -C xhci
	#0x4000
	make -s -C disk
	#one cat to rule them all
	cat cpu/temp jarvis/temp ahci/temp xhci/temp disk/temp > init
clean:
	make clean -s -C cpu
	make clean -s -C jarvis
	make clean -s -C ahci
	make clean -s -C xhci
	make clean -s -C disk
	rm -f init

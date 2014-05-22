init:
	make -s -C cpu
	make -s -C jarvis
	make -s -C ahci
	make -s -C xhci
	cat cpu/temp jarvis/temp ahci/temp xhci/temp > init
clean:
	make clean -s -C cpu
	make clean -s -C jarvis
	make clean -s -C ahci
	make clean -s -C xhci
	rm -f init

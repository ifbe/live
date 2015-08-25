linux:
	gcc -c -fPIC -O2 start.c
	gcc -c -fPIC -O2 ide.c
	gcc -c -fPIC -O2 ahci.c
	gcc -c -fPIC -O2 xhci.c
	gcc -c -fPIC -O2 usb.c
cross:
	x86_64-elf-gcc -c -fPIC -O2 start.c
	x86_64-elf-gcc -c -fPIC -O2 ide.c
	x86_64-elf-gcc -c -fPIC -O2 ahci.c
	x86_64-elf-gcc -c -fPIC -O2 xhci.c
	x86_64-elf-gcc -c -fPIC -O2 usb.c

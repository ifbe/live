linux:
	gcc -c -fPIC -O2 ehci.c
	gcc -c -fPIC -O2 xhci.c
	gcc -c -fPIC -O2 isr.c
cross:
	x86_64-elf-gcc -c -fPIC -O2 ehci.c
	x86_64-elf-gcc -c -fPIC -O2 xhci.c
	x86_64-elf-gcc -c -fPIC -O2 isr.c

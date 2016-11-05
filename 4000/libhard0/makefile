linux:
	gcc -c -fPIC -O2 cpu/fp.c
	gcc -c -fPIC -O2 cpu/port.c
	gcc -c -fPIC -O2 hc/ahci.c
	gcc -c -fPIC -O2 hc/ide.c
	gcc -c -fPIC -O2 hc/xhcihost.c
	gcc -c -fPIC -O2 hc/xhciport.c
cross:
	x86_64-elf-gcc -c -fPIC -O2 cpu/fp.c
	x86_64-elf-gcc -c -fPIC -O2 cpu/port.c
	x86_64-elf-gcc -c -fPIC -O2 hc/ahci.c
	x86_64-elf-gcc -c -fPIC -O2 hc/ide.c
	x86_64-elf-gcc -c -fPIC -O2 hc/xhcihost.c
	x86_64-elf-gcc -c -fPIC -O2 hc/xhciport.c
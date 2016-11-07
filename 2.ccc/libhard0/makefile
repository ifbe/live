cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) cpu/fp.c
	$(compiler) $(cflag) cpu/port.c
	$(compiler) $(cflag) hc/ahci.c
	$(compiler) $(cflag) hc/ide.c
	$(compiler) $(cflag) hc/xhcihost.c
	$(compiler) $(cflag) hc/xhciport.c
clean:
	rm -f *.o

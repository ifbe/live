c:
	#controller
	gcc -c -fPIC -O2 ahci/ahci.c -o ahci.o
	#sata
	gcc -c -fPIC -O2 sata/identify.c -o identify.o
	gcc -c -fPIC -O2 sata/read.c -o read.o
	nasm -f elf64 sata/sata.s -o sata.o
	#partition table
	gcc -c -fPIC -O2 parttable/parttable.c -o parttable.o
	nasm -f elf64 parttable/mount.s -o mount.o
	#filesystem
	gcc -c -fPIC -O2 fs/fat.c -o fat.o
	nasm -f elf64 fs/fat.s -o fatvars.o
	gcc -c -fPIC -O2 fs/ntfs.c -o ntfs.o
	nasm -f elf64 fs/ntfs.s -o ntfsvars.o
	#link all
	gcc -c -fPIC -O2 else/start.c -o start.o
	gcc -c -fPIC -O2 else/print.c -o print.o
	gcc -c -fPIC -O2 else/else.c -o else.o
	ld -T else/link.ld \
	start.o print.o else.o \
	ahci.o \
	identify.o read.o sata.o \
	parttable.o mount.o \
	fat.o fatvars.o ntfs.o ntfsvars.o \
	-o temp.bin
	nasm else/16kb.s -o temp
clean:
	rm -f *.o
	rm -f temp*

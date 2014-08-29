c:
	gcc -c -fPIC -O2 start.c -o build/start.o
	#sata
	gcc -c -fPIC -O2 sata/identify.c -o build/identify.o
	gcc -c -fPIC -O2 sata/read.c -o build/read.o
	nasm -f elf64 sata/sata.s -o build/sata.o
	#partition table
	gcc -c -fPIC -O2 parttable/parttable.c -o build/parttable.o
	nasm -f elf64 parttable/mount.s -o build/mount.o
	#filesystem
	gcc -c -fPIC -O2 fs/fat.c -o build/fat.o
	gcc -c -fPIC -O2 fs/ntfs.c -o build/ntfs.o
	gcc -c -fPIC -O2 fs/ext.c -o build/ext.o
	nasm -f elf64 fs/fs.s -o build/fs.o
	#library
	gcc -c -fPIC -O2 else/print.c -o build/print.o
	gcc -c -fPIC -O2 else/else.c -o build/else.o
	#link all
	ld -T else/link.ld \
	build/start.o \
	build/print.o build/else.o \
	build/identify.o build/read.o build/sata.o \
	build/parttable.o build/mount.o \
	build/fat.o build/ntfs.o build/ext.o build/fs.o \
	-o temp.bin
	nasm else/16kb.s -o temp
clean:
	rm -f build/*
	rm -f temp*

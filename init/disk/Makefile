c:
	rm -rf build
	mkdir build
	gcc -c -fPIC -O2 start.c -o build/start.o
	#sata
	gcc -c -fPIC -O2 sata/identify.c -o build/identify.o
	gcc -c -fPIC -O2 sata/read.c -o build/read.o
	#partitiontable+filesystem
	gcc -c -fPIC -O2 fs/master.c -o build/master.o
	gcc -c -fPIC -O2 fs/fat.c -o build/fat.o
	gcc -c -fPIC -O2 fs/ntfs.c -o build/ntfs.o
	gcc -c -fPIC -O2 fs/ext.c -o build/ext.o
	#library
	gcc -c -fPIC -O2 else/print.c -o build/print.o
	gcc -c -fPIC -O2 else/remember.c -o build/remember.o
	gcc -c -fPIC -O2 else/else.c -o build/else.o
	#link all
	ld -T else/link.ld \
	build/start.o \
	build/print.o build/remember.o build/else.o \
	build/identify.o build/read.o \
	build/master.o \
	build/fat.o build/ntfs.o build/ext.o \
	-o temp.bin
	nasm else/size.s -o temp
clean:
	rm -rf build
	rm -f temp*

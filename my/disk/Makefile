binary:
	rm -rf build
	mkdir build
	make -s library
	make -s main
	ld -T link/link.ld \
	build/start.o build/print.o build/remember.o build/else.o \
	build/identify.o build/read.o \
	build/master.o \
	build/fat.o build/ntfs.o build/ext.o \
	-o temp.bin
	nasm link/size.s -o temp
library:
	gcc -c -fPIC -O2 lib/start.c -o build/start.o
	gcc -c -fPIC -O2 lib/print.c -o build/print.o
	gcc -c -fPIC -O2 lib/remember.c -o build/remember.o
	gcc -c -fPIC -O2 lib/else.c -o build/else.o
	gcc -c -fPIC -O2 lib/identify.c -o build/identify.o
	gcc -c -fPIC -O2 lib/read.c -o build/read.o
main:
	gcc -c -fPIC -O2 master/master.c -o build/master.o
	gcc -c -fPIC -O2 master/fat.c -o build/fat.o
	gcc -c -fPIC -O2 master/ntfs.c -o build/ntfs.o
	gcc -c -fPIC -O2 master/ext.c -o build/ext.o
clean:
	rm -rf build
	rm -f temp*

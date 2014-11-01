inlinux:
	rm -rf build
	mkdir build
	make -s library compiler=gcc
	make -s main compiler=gcc
	make -s binary linker=ld
crosscompile:
	rm -rf build
	mkdir build
	make -s library compiler=x86_64-elf-gcc
	make -s main compiler=x86_64-elf-gcc
	make -s binary linker=x86_64-elf-ld
library:
	$(compiler) -c -fPIC -O2 lib/start.c -o build/start.o
	$(compiler) -c -fPIC -O2 lib/print.c -o build/print.o
	$(compiler) -c -fPIC -O2 lib/remember.c -o build/remember.o
	$(compiler) -c -fPIC -O2 lib/else.c -o build/else.o
	$(compiler) -c -fPIC -O2 lib/identify.c -o build/identify.o
	$(compiler) -c -fPIC -O2 lib/read.c -o build/read.o
main:
	$(compiler) -c -fPIC -O2 master/master.c -o build/master.o
	$(compiler) -c -fPIC -O2 master/fat.c -o build/fat.o
	$(compiler) -c -fPIC -O2 master/ntfs.c -o build/ntfs.o
	$(compiler) -c -fPIC -O2 master/ext.c -o build/ext.o
binary:
	$(linker) -T link/link.ld \
	build/start.o build/print.o build/remember.o build/else.o \
	build/identify.o build/read.o \
	build/master.o \
	build/fat.o build/ntfs.o build/ext.o \
	-o disk.bin
clean:
	rm -rf build
	rm -f *.bin
	rm -f *.o

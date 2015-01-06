binary:
	gcc -c -fPIC -O2 master.c
	gcc -c -fPIC -O2 annoying.c
	gcc -c -fPIC -O2 fat.c
	gcc -c -fPIC -O2 ntfs.c
	gcc -c -fPIC -O2 ext.c
	rm -f main.a
	ar crs main.a master.o fat.o ntfs.o ext.o
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 master.c
	x86_64-elf-gcc -c -fPIC -O2 annoying.c
	x86_64-elf-gcc -c -fPIC -O2 fat.c
	x86_64-elf-gcc -c -fPIC -O2 ntfs.c
	x86_64-elf-gcc -c -fPIC -O2 ext.c
	rm -f main.a
	ar crs main.a master.o fat.o ntfs.o ext.o
linux:
	gcc -c -fPIC -O2 master.c
	gcc -c -fPIC -O2 annoying.c
	gcc -c -fPIC -O2 fat.c
	gcc -c -fPIC -O2 ntfs.c
	gcc -c -fPIC -O2 ext.c
	rm -f main.a
	ar crs main.a master.o annoying.o fat.o ntfs.o ext.o
mac:
	gcc -c -fPIC -O2 master.c
	gcc -c -fPIC -O2 annoying.c
	gcc -c -fPIC -O2 fat.c
	gcc -c -fPIC -O2 ntfs.c
	gcc -c -fPIC -O2 ext.c
	rm -f main.a
	ar crs main.a master.o annoying.o fat.o ntfs.o ext.o
win:
	gcc -c -O2 master.c
	gcc -c -O2 annoying.c
	gcc -c -O2 fat.c
	gcc -c -O2 ntfs.c
	gcc -c -O2 ext.c
	rm -f main.a
	ar crs main.a master.o annoying.o fat.o ntfs.o ext.o
clean:
	rm -f *.o *.a

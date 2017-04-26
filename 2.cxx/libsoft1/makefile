cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) acpi.c
	$(compiler) $(cflag) convert.c
	$(compiler) $(cflag) pt/parttable.c
	$(compiler) $(cflag) fs/fs.fat.c
	$(compiler) $(cflag) fs/fs.ntfs.c
	$(compiler) $(cflag) fs/fs.ext.c
clean:
	rm -f *.o

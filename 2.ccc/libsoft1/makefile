cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) parttable.c
	$(compiler) $(cflag) fat.c
	$(compiler) $(cflag) ntfs.c
	$(compiler) $(cflag) ext.c

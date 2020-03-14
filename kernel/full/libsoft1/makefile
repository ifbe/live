cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) pt/pt.gpt.c
	$(compiler) $(cflag) pt/pt.mbr.c
	$(compiler) $(cflag) pt/pt.c
	$(compiler) $(cflag) fs/fs.fat.c
	$(compiler) $(cflag) fs/fs.iso.c
	$(compiler) $(cflag) fs/fs.c
	$(compiler) $(cflag) str/compare.c
	$(compiler) $(cflag) str/convert.c
	$(compiler) $(cflag) tab/acpi.c
	$(compiler) $(cflag) artery.c
clean:
	rm -f *.o

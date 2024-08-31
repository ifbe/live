cflag := -c -fPIC -ffreestanding -fno-stack-protector -O2
target:
	$(CC) $(cflag) pt/pt.gpt.c
	$(CC) $(cflag) pt/pt.mbr.c
	$(CC) $(cflag) pt/pt.c
	$(CC) $(cflag) fs/fs.fat.c
	$(CC) $(cflag) fs/fs.iso.c
	$(CC) $(cflag) fs/fs.c
	$(CC) $(cflag) str/compare.c
	$(CC) $(cflag) str/convert.c
	$(CC) $(cflag) str/printf.c
	$(CC) $(cflag) tab/acpi.c
	$(CC) $(cflag) artery.c
clean:
	rm -f *.o

cflag := -c -fPIC -ffreestanding -fno-stack-protector -O2
target:
	$(CC) $(cflag) file/disk.c
	$(CC) $(cflag) system.c
clean:
	rm -f *.o

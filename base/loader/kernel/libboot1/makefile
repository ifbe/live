cflag := -c -fPIC -ffreestanding -fno-stack-protector -O2
target:
	$(CC) $(cflag) loop/poller.c
	$(CC) $(cflag) debug.c
clean:
	rm -f *.o

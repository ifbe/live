cflag := -c -fPIC -fno-stack-protector -O2
target:
	$(CC) $(cflag) loop/poller.c
	$(CC) $(cflag) dbg/say.c
	$(CC) $(cflag) debug.c
clean:
	rm -f *.o

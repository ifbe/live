cflag := -c -fPIC -ffreestanding -fno-stack-protector -O2
target:
	$(CC) $(cflag) forcli/say.c
	$(CC) $(cflag) forrgba/ascii.c
	$(CC) $(cflag) term/term.context.c
	$(CC) $(cflag) term/term.server.c
	$(CC) $(cflag) entity.c
clean:
	rm -f *.o

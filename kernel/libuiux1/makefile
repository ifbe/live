cflag := -c -fPIC -fno-stack-protector -O2
target:
	$(CC) $(cflag) forrgba/ascii.c
	$(CC) $(cflag) term/term.c
	$(CC) $(cflag) term/term.input.c
	$(CC) $(cflag) entity.c
clean:
	rm -f *.o

cflag := -c -fPIC -ffreestanding -fno-stack-protector -O2
target:
	$(CC) $(cflag) cli/cli.c
	$(CC) $(cflag) fb/fb.c
	$(CC) $(cflag) supply.c
clean:
	rm -f *.o

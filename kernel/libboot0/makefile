AS :=
cflag := -c -fPIC -fno-stack-protector -O2

.PHONY:rpi
rpi:
	$(AS) -c rpi/entry.s -o entry.o
	$(CC) $(cflag) rpi/main.c
	$(CC) $(cflag) basic.c

.PHONY:x64
x64:
	$(CC) $(cflag) x64/main.c
	$(CC) $(cflag) basic.c

clean:
	rm -f *.o

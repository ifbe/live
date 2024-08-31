cflag := -c -fPIC -ffreestanding -fno-stack-protector -O2
target:
	$(CC) $(cflag) kbd/kbd.c
	$(CC) $(cflag) net/net.c
	$(CC) $(cflag) sata/identify.c
	$(CC) $(cflag) uart/uart.c
	$(CC) $(cflag) driver.c
clean:
	rm -f *.o

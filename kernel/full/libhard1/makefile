cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) sata/identify.c
	$(compiler) $(cflag) kbd/kbd.c
	$(compiler) $(cflag) uart/uart.c
	$(compiler) $(cflag) driver.c
clean:
	rm -f *.o

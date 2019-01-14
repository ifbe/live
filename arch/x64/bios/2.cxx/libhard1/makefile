cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) body.c
	$(compiler) $(cflag) identify.c
	$(compiler) $(cflag) kbd.c
	$(compiler) $(cflag) read.c
	$(compiler) $(cflag) uart.c
clean:
	rm -f *.o

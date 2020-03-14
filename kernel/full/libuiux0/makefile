cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) fb/fb.c
	$(compiler) $(cflag) supply.c
clean:
	rm -f *.o

cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) debug.c
	$(compiler) $(cflag) main.c
	$(compiler) $(cflag) say.c
clean:
	rm -f *.o

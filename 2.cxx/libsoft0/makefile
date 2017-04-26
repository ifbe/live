cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) haha.c
clean:
	rm -f *.o

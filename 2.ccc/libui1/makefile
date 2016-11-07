cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) ascii.c
	$(compiler) $(cflag) character.c
clean:
	rm -f *.o

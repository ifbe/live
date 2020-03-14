cflag := -c -fPIC -fno-stack-protector -O2
linux:
	make target compiler=gcc
cross:
	make target compiler=x86_64-elf-gcc
target:
	$(compiler) $(cflag) help/ascii.c
	$(compiler) $(cflag) item/item.terminal.c
	$(compiler) $(cflag) entity.c
clean:
	rm -f *.o

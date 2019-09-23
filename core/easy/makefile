cflag := -c -fPIC -fno-stack-protector -O2
binary:
	gcc $(cflag) start.c
	x86_64-elf-ld -T link.ld -o cxx.bin start.o
cross:
	x86_64-elf-gcc $(cflag) start.c
	x86_64-elf-ld -T link.ld -o cxx.bin start.o
clean:
	rm -f */*.o *.bin

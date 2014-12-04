binary:
	gcc -c -fPIC -O2 jpeg.c
	gcc -c -fPIC -O2 decode.c
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 jpeg.c
	x86_64-elf-gcc -c -fPIC -O2 decode.c
linux:
	gcc -c -fPIC -O2 jpeg.c
	gcc -c -fPIC -O2 decode.c
mac:
	clang -c -fPIC -O2 jpeg.c
	clang -c -fPIC -O2 decode.c
win:
	gcc -c -O2 jpeg.c
	gcc -c -O2 decode.c

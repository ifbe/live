.PHONY:binary
binary:
	gcc -c -fPIC -O2 bare/start.c
	nasm -f elf64 bare/bin.s -o bin.o
	gcc -c -fPIC -O2 bare/input.c
	gcc -c -fPIC -O2 bare/malloc.c
	gcc -c -fPIC -O2 bare/screen.c
	rm -f wrap.a
	ar crs wrap.a bin.o input.o malloc.o screen.o
.PHONY:crosscompile
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 bare/start.c
	nasm -f elf64 bare/bin.s -o bin.o
	x86_64-elf-gcc -c -fPIC -O2 bare/input.c
	x86_64-elf-gcc -c -fPIC -O2 bare/malloc.c
	x86_64-elf-gcc -c -fPIC -O2 bare/screen.c
	rm -f wrap.a
	ar crs wrap.a bin.o input.o malloc.o screen.o
.PHONY:linux
linux:
	gcc -c -fPIC -O2 system/file.c
	gcc -c -fPIC -O2 system/malloc.c
	gcc -c -fPIC -O2 system/screen.c
	gcc -c -fPIC -O2 system/sdl.c
	rm -f wrap.a
	ar crs wrap.a file.o malloc.o screen.o sdl.o
.PHONY:mac
mac:
	clang -c -fPIC -O2 system/file.c
	clang -c -fPIC -O2 system/malloc.c
	clang -c -fPIC -O2 system/screen.c
	clang -c -fPIC -O2 system/sdl.c -I /usr/local/Cellar/sdl2/2.0.3/include
	rm -f wrap.a
	ar crs wrap.a file.o malloc.o screen.o sdl.o
.PHONY:win
win:
	gcc -c -O2 system/file.c
	gcc -c -O2 system/malloc.c
	gcc -c -O2 system/screen.c
	gcc -c -O2 system/sdl.c -I /opt/SDL2-2.0.3/x86_64-w64-mingw32/include
	rm -f wrap.a
	ar crs wrap.a file.o malloc.o screen.o sdl.o

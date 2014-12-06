.PHONY:binary
binary:
	gcc -c -fPIC -O2 bare/start.c
	gcc -c -fPIC -O2 bare/screen.c
	gcc -c -fPIC -O2 bare/malloc.c
	gcc -c -fPIC -O2 bare/input.c
	nasm -f elf64 bare/bin.s -o bin.o
.PHONY:crosscompile
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 bare/start.c
	x86_64-elf-gcc -c -fPIC -O2 bare/screen.c
	x86_64-elf-gcc -c -fPIC -O2 bare/malloc.c
	x86_64-elf-gcc -c -fPIC -O2 bare/input.c
	nasm -f elf64 bare/bin.s -o bin.o
.PHONY:linux
linux:
	gcc -c -fPIC -O2 system/global.c
	gcc -c -fPIC -O2 system/input.c
	gcc -c -fPIC -O2 system/screen.c
	gcc -c -fPIC -O2 system/malloc.c
	gcc -c -fPIC -O2 system/disk.c
	ar crs wrap.a global.o input.o screen.o malloc.o disk.o
.PHONY:mac
mac:
	clang -c -fPIC -O2 system/global.c
	clang -c -fPIC -O2 system/input.c -I /usr/local/Cellar/sdl2/2.0.3/include
	clang -c -fPIC -O2 system/screen.c -I /usr/local/Cellar/sdl2/2.0.3/include
	clang -c -fPIC -O2 system/malloc.c
	clang -c -fPIC -O2 system/disk.c
	ar crs wrap.a global.o input.o screen.o malloc.o disk.o
.PHONY:win
win:
	gcc -c -O2 system/global.c
	gcc -c -O2 system/input.c -I /opt/SDL2-2.0.3/x86_64-w64-mingw32/include
	gcc -c -O2 system/screen.c -I /opt/SDL2-2.0.3/x86_64-w64-mingw32/include
	gcc -c -O2 system/malloc.c
	gcc -c -O2 system/disk.c
	ar crs wrap.a global.o input.o screen.o malloc.o disk.o

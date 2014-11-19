.PHONY:bare
bare:
	gcc -c -fPIC -O2 bare/start.c
	gcc -c -fPIC -O2 bare/print.c
	gcc -c -fPIC -O2 bare/malloc.c
	gcc -c -fPIC -O2 bare/input.c
	nasm -f elf64 bare/bin.s -o bin.o
.PHONY:cross
cross:
	x86_64-elf-gcc -c -fPIC -O2 bare/start.c
	x86_64-elf-gcc -c -fPIC -O2 bare/print.c
	x86_64-elf-gcc -c -fPIC -O2 bare/malloc.c
	x86_64-elf-gcc -c -fPIC -O2 bare/input.c
	nasm -f elf64 bare/bin.s -o bin.o
.PHONY:linux
linux:
	gcc -c -fPIC -O2 system/global.c
	gcc -c -fPIC -O2 system/sdl.c
	gcc -c -fPIC -O2 system/memory.c
	gcc -c -fPIC -O2 system/disk.c
	ar crs wrap.a global.o sdl.o memory.o disk.o
.PHONY:mac
mac:
	clang -c -fPIC -O2 system/global.c
	clang -c -fPIC -O2 system/sdl.c -I /usr/local/Cellar/sdl2/2.0.3/include
	clang -c -fPIC -O2 system/memory.c
	clang -c -fPIC -O2 system/disk.c
	ar crs wrap.a global.o sdl.o memory.o disk.o
.PHONY:win
win:
	gcc -c -O2 system/global.c
	gcc -c -O2 system/sdl.c -I /opt/SDL2-2.0.3/x86_64-w64-mingw32/include
	gcc -c -O2 system/memory.c
	gcc -c -O2 system/disk.c
	ar crs wrap.a global.o sdl.o memory.o disk.o

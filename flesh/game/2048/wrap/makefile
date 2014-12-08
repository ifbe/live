.PHONY:binary
binary:
	gcc -c -fPIC -O2 bare/start.c
	gcc -c -fPIC -O2 bare/screen.c
	gcc -c -fPIC -O2 bare/random.c
	gcc -c -fPIC -O2 bare/input.c
	ar crs wrap.a screen.o random.o input.o
.PHONY:crosscompile
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 bare/start.c
	x86_64-elf-gcc -c -fPIC -O2 bare/screen.c
	x86_64-elf-gcc -c -fPIC -O2 bare/random.c
	x86_64-elf-gcc -c -fPIC -O2 bare/input.c
	ar crs wrap.a screen.o random.o input.o
.PHONY:linux
linux:
	gcc -c system/global.c
	gcc -c system/input.c
	gcc -c system/screen.c
	ar crs wrap.a global.o input.o screen.o
.PHONY:mac
mac:
	clang -c system/global.c
	clang -c system/input.c -I /usr/local/Cellar/sdl2/2.0.3/include
	clang -c system/screen.c -I /usr/local/Cellar/sdl2/2.0.3/include
	ar crs wrap.a global.o input.o screen.o
.PHONY:win
win:
	gcc -c system/global.c
	gcc -c system/input.c -I /opt/SDL2-2.0.3/x86_64-w64-mingw32/include
	gcc -c system/screen.c -I /opt/SDL2-2.0.3/x86_64-w64-mingw32/include
	gcc -c system/random.c
	ar crs wrap.a global.o input.o screen.o random.o
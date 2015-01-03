.PHONY:bare
binary:
	gcc -c -fPIC -O2 bare/start.c
	gcc -c -fPIC -O2 bare/screen.c
	gcc -c -fPIC -O2 bare/input.c
	rm -f wrap.a
	ar crs wrap.a screen.o input.o
.PHONY:cross
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 bare/start.c
	x86_64-elf-gcc -c -fPIC -O2 bare/screen.c
	x86_64-elf-gcc -c -fPIC -O2 bare/input.c
	rm -f wrap.a
	ar crs wrap.a screen.o input.o
.PHONY:linux
linux:
	gcc -c -fPIC -O2 system/screen.c
	gcc -c -fPIC -O2 system/sdl.c
	rm -f wrap.a
	ar crs wrap.a screen.o sdl.o
.PHONY:mac
mac:
	clang -c -fPIC -O2 system/screen.c
	clang -c -fPIC -O2 system/sdl.c -I /usr/local/Cellar/sdl2/2.0.3/include
	rm -f wrap.a
	ar crs wrap.a screen.o sdl.o
.PHONY:win
win:
	gcc -c -O2 system/screen.c
	gcc -c -O2 system/sdl.c -I d:\msys\lib\SDL2-2.0.3\x86_64-w64-mingw32\include
	rm -f wrap.a
	ar crs wrap.a screen.o sdl.o

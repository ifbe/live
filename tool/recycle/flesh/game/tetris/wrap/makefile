.PHONY:binary
binary:
	gcc -c -fPIC -O2 bare/start.c
	gcc -c -fPIC -O2 bare/screen.c
	gcc -c -fPIC -O2 bare/event.c
	gcc -c -fPIC -O2 bare/random.c
	nasm -f elf64 bare/wrap8.s -o wrap8.o
	gcc -c -fPIC -O2 bare/irq8.c
	rm -f wrap.a
	ar crs wrap.a wrap8.o irq8.o screen.o event.o random.o
.PHONY:crosscompile
crosscompile:
	x86_64-elf-gcc -c -fPIC -O2 bare/start.c
	x86_64-elf-gcc -c -fPIC -O2 bare/screen.c
	x86_64-elf-gcc -c -fPIC -O2 bare/event.c
	x86_64-elf-gcc -c -fPIC -O2 bare/random.c
	nasm -f elf64 bare/wrap8.s -o wrap8.o
	x86_64-elf-gcc -c -fPIC -O2 bare/irq8.c
	rm -f wrap.a
	ar crs wrap.a wrap8.o irq8.o screen.o event.o random.o
.PHONY:linux
linux:
	gcc -c system/screen.c
	gcc -c system/sdl.c
	rm -f wrap.a
	ar crs wrap.a sdl.o screen.o
.PHONY:mac
mac:
	clang -c system/screen.c
	clang -c system/sdl.c -I /usr/local/Cellar/sdl2/2.0.3/include
	rm -f wrap.a
	ar crs wrap.a event.o screen.o
.PHONY:win
win:
	gcc -c system/random.c
	gcc -c system/screen.c
	gcc -c system/sdl.c -I /opt/SDL2-2.0.3/x86_64-w64-mingw32/include
	rm -f wrap.a
	ar crs wrap.a sdl.o screen.o random.o
.PHONY:wrong
wrong:
	gcc -c wrong/wrong.c
	rm -f wrap.a
	ar crs wrap.a wrong.o

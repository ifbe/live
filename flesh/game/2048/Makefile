binary:
	make -s -C main bare
	make -s -C wrap bare
	ld -T link/link.ld \
	start.o print.o hlt.o \
	2048.o \
	-o 2048.bin
binarycross:
	make -s -C main cross
	make -s -C wrap cross
	ld -T link/link.ld \
	start.o print.o hlt.o \
	2048.o \
	-o 2048.bin
win:
	make -s -C main win
	make -s -C wrap win
	gcc main/2048.o main/anscii.o \
	-mwindows -Wl,--no-undefined -static -static-libgcc -L /opt/SDL2-2.0.3/x86_64-w64-mingw32/lib \
	wrap/wrap.a -lmingw32 -lSDL2 \
	-lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid
windynamic:
	make -s -C main win
	make -s -C wrap win
	gcc main/2048.o main/anscii.o \
	-mwindows -L /opt/SDL2-2.0.3/x86_64-w64-mingw32/lib \
	wrap/wrap.a -lmingw32 -lSDL2
linux:
	make -s -C main linux
	make -s -C wrap linux
	gcc main/2048.o main/anscii.o \
	wrap/wrap.a -lSDL2 -lm
linuxdynamic:
	make -C main linux
	make -C wrap linux
	gcc main/2048.o main/anscii.o \
	wrap/wrap.a -lSDL2 -lm
mac:
	make -C main mac
	make -C wrap mac
	clang main/2048.o main/anscii.o \
	-L /usr/local/Cellar/sdl2/2.0.3/lib \
	wrap/wrap.a -lSDL2 -lm
macdynamic:
	make -C main mac
	make -C wrap mac
	clang main/2048.o main/anscii.o \
	-L /usr/local/Cellar/sdl2/2.0.3/lib \
	wrap/wrap.a -lSDL2 -lm
clean:
	rm -f */*.o *.bin *.exe *.out
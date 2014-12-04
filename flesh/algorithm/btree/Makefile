binary:
	make -s -C wrap binary
	make -s -C main binary
	ld -T link.ld \
	wrap/start.o wrap/screen.o wrap/input.o \
	main/btree.o main/anscii.o \
	-o btree.bin
crosscompile:
	make -s -C wrap crosscompile
	make -s -C main crosscompile
	x86_64-elf-ld -T link.ld \
	wrap/start.o wrap/screen.o wrap/input.o \
	main/btree.o main/anscii.o \
	-o btree.bin
linux:
	make -s -C wrap linux
	make -s -C main linux
	gcc main/btree.o main/anscii.o \
	wrap/wrap.a -lSDL2 -lm
mac:
	make -s -C wrap mac
	make -s -C main mac
	gcc main/btree.o main/anscii.o \
	-L /usr/local/Cellar/sdl2/2.0.3/lib \
	wrap/wrap.a -lSDL2 -lm
win:
	make -s -C wrap win
	make -s -C main win
	gcc main/btree.o main/anscii.o \
	-Wl,--no-undefined -static -static-libgcc -L /opt/SDL2-2.0.3/x86_64-w64-mingw32/lib \
	wrap/wrap.a -lmingw32 -lSDL2main -lSDL2 \
	-lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid
clean:
	rm -f */*.o */*.a *.bin *.out *.exe

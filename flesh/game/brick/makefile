win:
	make -s -C wrap win
	make -s -C main win
	gcc main/brick.o \
	-mwindows -Wl,--no-undefined -static -static-libgcc -L /opt/SDL2-2.0.3/x86_64-w64-mingw32/lib \
	wrap/wrap.a -lmingw32 -lSDL2 \
	-lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid
windynamic:
	make -s -C wrap win
	make -s -C main win
	gcc main/brick.o \
	-mwindows -L /opt/SDL2-2.0.3/x86_64-w64-mingw32/lib \
	wrap/wrap.a -lmingw32 -lSDL2
linux:
	make -s -C wrap linux
	make -s -C main linux
	gcc main/brick.o \
	wrap/wrap.a -lSDL2 -lm
mac:
	make -s -C wrap mac
	make -s -C main mac
	clang main/brick.o \
	-L /usr/local/Cellar/sdl2/2.0.3/lib \
	wrap/wrap.a -lSDL2 -lm
wrong:
	make -s -C wrap wrong
	make -s -C main linux
	gcc main/brick.o wrap/wrap.a
binary:
	make -s -C main binary
	make -s -C wrap binary
	ld -T link.ld -o brick.bin wrap/start.o \
	main/brick.o \
	wrap/wrap.a
crosscompile:
	make -s -C main crosscompile
	make -s -C wrap crosscompile
	x86_64-elf-ld -T link.ld -o brick.bin wrap/start.o \
	main/brick.o \
	wrap/wrap.a
qemu:
	cp ../../../core/core.bin test.img    #first64k
	dd if=brick.bin of=test.img bs=65536 seek=1 conv=notrunc    #real bin
	echo -n 9 | dd of=test.img bs=1 seek=1048575 conv=notrunc    #fill 0
	echo "test" | dd of=test.img bs=1 seek=448 conv=notrunc    #test sign
	../../../tool/qemu $(shell pwd)/test.img
kvm:
	cp ../../../core/core.bin test.img    #first64k
	dd if=brick.bin of=test.img bs=65536 seek=1 conv=notrunc    #real bin
	echo -n 9 | dd of=test.img bs=1 seek=1048575 conv=notrunc    #fill 0
	echo "test" | dd of=test.img bs=1 seek=448 conv=notrunc    #test sign
	../../../tool/kvm $(shell pwd)/test.img
clean:
	rm -f */*.o */*.a *.bin *.img *.exe *.out

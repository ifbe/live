binary:
	make -s -C wrap bare
	make -s -C main linux
	ld -T link.ld \
	wrap/start.o wrap/print.o wrap/malloc.o \
	main/bin.o main/jpeg.o main/decode.o \
	-o jpeg.bin
linux:
	make -s -C wrap linux
	make -s -C main linux
	gcc \
	wrap/sdl.o wrap/malloc.o \
	main/bin.o main/jpeg.o main/decode.o \
	-lSDL2 -lm

clean:
	rm -f */*.o *.bin *.out

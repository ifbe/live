inlinux:
	make -s library
	make -s main
	make -s binary
library:
	gcc -c lib/start.c
main:
	gcc -c -O2 taiji.c
binary:
	ld -T link/link.ld \
	start.o \
	taiji.o \
	-o taiji.bin
clean:
	rm -f *.o
	rm -f *.bin

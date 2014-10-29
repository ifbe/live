binary:
	make -s library
	make -s main
	ld -T link/link.ld \
	start.o print.o \
	ahci.o \
	-o ahci.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
main:
	gcc -c -fPIC -O2 ahci.c
clean:
	rm -f *.o
	rm -f *.bin

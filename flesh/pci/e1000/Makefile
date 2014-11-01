binary:
	make -s library
	make -s c
	ld -T link/link.ld \
	start.o print.o remember.o interrupt.o \
	eth.o  \
	-o eth.bin
library:
	gcc -c -fPIC -O2 lib/start.c
	gcc -c -fPIC -O2 lib/print.c
	gcc -c -fPIC -O2 lib/remember.c
	nasm -f elf64 lib/interrupt.s -o interrupt.o
c:
	gcc -c -fPIC -O2 eth.c
clean:
	rm -f *.o
	rm -f *.bin

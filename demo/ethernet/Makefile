c:
	gcc -c -fPIC -O2 start.c
	gcc -c -fPIC -O2 ethernet.c
	gcc -c -fPIC -O2 print.c
	gcc -c -fPIC -O2 remember.c
	ld -T link.ld start.o ethernet.o print.o remember.o -o temp.bin
	nasm size.s -o ethernet
test:
	make -s c
	make -C ../.. mountfat
	sudo cp ethernet /mnt/nbd/live
	make -C ../.. umountfat
	make -C ../.. fattest
clean:
	rm -f *.o
	rm -f temp*
	rm -f ethernet

multitask:
	gcc -c -O2 start.c
	gcc -c -O2 xiangqi.c
	ld -T link.ld start.o xiangqi.o -o xiangqi
clean:
	rm -f *.o xiangqi

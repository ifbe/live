init:
	#第一部分占据[0,0x1fff]
	#作用是初始化cpu到longmode以及设置开启一些东西
	make -s -C cpu
	#第二部分占据[0x2000,0xffff]
	#其实你可以直接删除my文件夹，写一个任何样子的，链接成二进制格式
	#第一部分运行初始化完毕会jump到第一部分的尾巴上，也就是my文件的开头
	#不用管在哪儿(一般被加载器扔在0x8000)，写成地址无关指令就行
	make -s -C my
	#把两个文件连在一起
	cat cpu/cpu my/my > init
clean:
	make clean -s -C cpu
	make clean -s -C my
	rm -f init

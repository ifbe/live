all:
	make -s -C jarvis	#[0x2000,0x3fff]
	make -s -C ahci		#[0x4000,0x5fff]
	make -s -C xhci		#[0x6000,0x7fff]
	make -s -C usb		#[0x8000,0xbfff]
	make -s -C disk		#[0xc000,0xffff]
	nasm my.s -o my.bin
clean:
	make clean -s -C jarvis
	make clean -s -C ahci
	make clean -s -C xhci
	make clean -s -C usb
	make clean -s -C disk
	rm -f *.bin


#[0x100000,0x10ffff]:/
#格式：[+0,+7]:名字,[+8,+f]:地址
#[0x110000,0x11ffff]:/acpi
#格式：[+0,+7]:名字,[+8,+f]:地址
#[0x120000,0x12ffff]:/console
#格式：全是anscii码，每0x128字节一行，总共30行屏幕
#[0x130000,0x13ffff]:/journal
#格式：全是anscii码，各种程序留下的log

#[0x140000,0x14ffff]:/pci
#格式：[+0,+3]:pidvid,[+8,+b]:设备类型代码,[+10,+14]:地址,[+18,+1b]:设备是什么
#[0x150000,0x15ffff]:/usb
#格式：[+0,+3]:pidvid,[+8,+b]:设备类型,[+10,+17]:rootport,[+18,+20]:routestring

#[0x140000,0x14ffff]:/todo
#[+0,+7]名字,[+8,+f]:函数地址(这里每一条，每当核心有空就会call)
#[0x180000,0x1fffff]:/bin
#格式：[+0,+7]:函数名字,[+8,+f]:函数地址

#[0x200000,0x2fffff]:ahci驱动需要的空间
#[0x300000,0x3fffff]:xhci驱动需要的空间
#[0x400000,0x4fffff]:几个文件系统需要的空间
#[0x500000,0x5fffff]:usb的context，transfer ring需要的空间

#[0x1000000,0x13fffff]:准备写到屏幕上的点阵rgb数据（f1）
#[0x1400000,0x17fffff]:准备写到屏幕上的点阵rgb数据（f2）
#[0x1800000,0x1bfffff]:准备写到屏幕上的点阵rgb数据（f3）
#[0x1c00000,0x1ffffff]:准备写到屏幕上的点阵rgb数据（f4）

#[0x2000000,?]:磁盘上指定的文件被扔到这里

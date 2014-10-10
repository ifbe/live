all:
	make -s -C jarvis	#[0x2000,0x3fff]
	make -s -C ahci		#[0x4000,0x5fff]
	make -s -C xhci		#[0x6000,0x7fff]
	make -s -C usb		#[0x8000,0xbfff]
	make -s -C disk		#[0xc000,0xffff]
	cat jarvis/temp ahci/temp xhci/temp usb/temp disk/temp > my

clean:
	make clean -s -C jarvis
	make clean -s -C ahci
	make clean -s -C xhci
	make clean -s -C usb
	make clean -s -C disk
	rm -f my




#[0x40000,0x7ffff]:/journal
#格式：全是anscii码，各种程序留下的log

#[0x100000,0x10ffff]:/acpi
#格式：[+0,+7]:表名字,[+8,+f]:表地址

#[0x110000,0x11ffff]:/pci
#格式：[+0,+3]:pidvid,[+8,+b]:设备类型代码,[+10,+14]:地址,[+18,+1b]:设备是什么

#[0x120000,0x12ffff]:/console
#格式：全是anscii码，每0x128字节一行，总共30行屏幕

#[0x180000,0x1fffff]:/bin
#格式：[+0,+7]:函数名字,[+8,+f]:函数地址

#[0x200000,0x2fffff]:ahci驱动需要的空间
#[0x300000,0x3fffff]:xhci驱动需要的空间
#[0x400000,0x4fffff]:几个文件系统需要的空间

#[0x1000000,0x13fffff]:准备写到屏幕上的点阵rgb数据（f1）
#[0x1400000,0x17fffff]:准备写到屏幕上的点阵rgb数据（f2）
#[0x1800000,0x1bfffff]:准备写到屏幕上的点阵rgb数据（f3）
#[0x1c00000,0x1ffffff]:准备写到屏幕上的点阵rgb数据（f4）

#[0x2000000,?]:test指令会把磁盘上指定的文件扔到这里然后执行

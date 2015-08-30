#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define idehome 0x100000
#define ahcihome 0x100000

#define journalhome 0xd00000
#define journalsize 0x100000




void diary(char*,...);

void initahci();
void initide();
void initxhci();
void initusb();

void master();




void start()
{
	//
	QWORD* p=(QWORD*)(journalhome);
	QWORD temp;
	for(temp=0;temp<journalsize/8;temp++) p[temp]=0;
	diary("oh we have found a whole new world,landing...%x...%x...%x...",1,2,3);

	//初始化，然后把找到的设备列一个清单
	initahci();

	//初始化，然后放进清单
	initide();

	//xhcihost,xhciport(hub+基本的询问)
	initxhci();
	initusb();

	//自己找磁盘,找到了就自己挂分区
	master();
}

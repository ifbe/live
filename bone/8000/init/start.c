#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define idehome 0x100000
#define ahcihome 0x100000

#define journalhome 0xd00000
#define journalsize 0x100000




void say(char*,...);

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
	say("oh we have found a whole new world,landing...%x...%x...%x...",1,2,3);

	//有ahci就用ahci
	initahci();

	//没有发现就用ide
	temp=*(QWORD*)(ahcihome);
	if( temp == 0 ) initide();

	//xhci
	initxhci();
	initusb();

	//什么都没有就报错返回
	temp=*(QWORD*)(ahcihome);
	if( temp == 0 )
	{
		say("no disk");
		return;
	}

	//读分区
	master();
}

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define journalhome 0xd00000
#define journalsize 0x100000




void say(char*,...);

void initahci();
void master();




void start()
{
	//
	QWORD* p=(QWORD*)(journalhome);
	QWORD temp;
	for(temp=0;temp<journalsize/8;temp++) p[temp]=0;
	say("oh we have found a whole new world,landing...%x...%x...%x...",1,2,3);

	//检查问题
	initahci();
	if( (*(QWORD*)(0x100000+8)) == 0 )
	{
		say("no disk");
		return;
	}

	//读分区
	master();
}

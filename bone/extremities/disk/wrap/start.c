#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long




void start()
{
	//检查问题
	QWORD diskaddr=*(QWORD*)(0x200000+8);
	if(diskaddr==0)
	{
		say("no disk");
		return;
	}

	//读分区
	master();
}

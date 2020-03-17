#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
//libui
void entitycreate(void*);
void entitydelete();
void supplycreate(void*);
void supplydelete();
//libsoft
void arterycreate(void*);
void arterydelete();
void systemcreate(void*);
void systemdelete();
//libhard
void drivercreate(void*);
void driverdelete();
void devicecreate(void*);
void devicedelete();
//libboot
void debugcreate(void*);
void debugdelete();
void basiccreate(void*);
void basicdelete();
//
void termcreate();
//
int poller();
void say(void*, ...);




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static u8*     basic=0;		//4m
static u8*      body=0;		//4m
static u8*    memory=0;		//4m
static u8* character=0;		//4m




void main()
{
	void* haha = (void*)0x1000000;
	basic = haha + 0;
	body = haha + 0x400000;
	memory = haha + 0x800000;
	character = haha + 0xc00000;

	//[0,4)：构架相关，以及内核日志
	basiccreate( basic );
	debugcreate( basic );

	//[4,7)：硬件驱动，以及底层协议栈
	devicecreate( body );
	drivercreate( body );

	//[8,c)：文件读写，以及详细分析
	systemcreate( memory );
	arterycreate( memory );

	//[c,f)：窗口开闭，以及用户界面
	supplycreate( character );
	entitycreate( character );

	//
	termcreate();

	//go
	poller();
}

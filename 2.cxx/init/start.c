#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
//libui
void charactercreate(void*,void*);
void characterdelete();
void displaycreate(void*,void*);
void displaydelete();
//libsoft
void arterycreate(void*,void*);
void arterydelete();
void systemcreate(void*,void*);
void systemdelete();
//libhard
void bodycreate(void*,void*);
void bodydelete();
void drivercreate(void*,void*);
void driverdelete();
//libboot
void debugcreate(void*,void*);
void debugdelete();
void basiccreate(void*,void*);
void basicdelete();
//
void say(void*, ...);
void main();




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static u8*     basic=0;		//4m
static u8*      body=0;		//4m
static u8*    memory=0;		//4m
static u8* character=0;		//4m




void start()
{
	int j;
	u8* haha = (void*)(0x20000);
	for(j=0;j<0x20000;j++)haha[j] = 0;

	haha = (void*)0x1000000;
	basic = haha + 0;
	body = haha + 0x400000;
	memory = haha + 0x800000;
	character = haha + 0xc00000;

	//[0,4)：构架相关，以及内核日志
	basiccreate( 0 , basic );
	debugcreate( 0 , basic );

	//[4,7)：硬件驱动，以及底层协议栈
	drivercreate( 0 , body );
	bodycreate( 0 , body );

	//[8,c)：文件读写，以及详细分析
	systemcreate( 0 , memory );
	arterycreate( 0 , memory );

	//[c,f)：窗口开闭，以及用户界面
	displaycreate( 0 , character );
	charactercreate( 0 , character );

	//go
	main();

	//after
	__asm__("int $0x3");
}

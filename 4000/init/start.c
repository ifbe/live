#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define journalhome 0xd00000
#define window 0x1000000
//libboot
void initconsole();
void diary(char*,...);
//libhard
void initahci();
void initide();
void initxhci();
//libsoft
void initparttable();
void initfilesystem();
//libui0
void initscreen(u64 addr);
void writescreen();
//libui1
void initcharacter(u64 addr);
void showprint();
//
void main();



void start()
{
	//0.清空日志内存
	u64* p=(u64*)(journalhome);
	u64 temp;
	for(temp=0;temp<0x20000;temp++) p[temp]=0;
	diary("oh we have found a whole new world,landing...%x...%x...%x...",3,2,1);

	//before
	initahci();
	initide();
	initxhci();

	initscreen(window);
	initcharacter(window);

	main();




	//
	while(1)
	{
		showprint();
		writescreen();
	}




	//after
	__asm__("int $0x3");
}

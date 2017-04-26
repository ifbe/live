#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define journalhome 0x100000	//1m
#define window 0x1000000		//16m
//libboot
void initconsole();
void diary(char*,...);
//libhard
void initahci();
void initide();
void initxhci();
//libsoft
void initacpi();
void initparttable();
void initfilesystem();
//libui0
void initscreen(u64 addr);
void writescreen();
//libui1
void initcharacter(u64 addr);
void characterread();
void characterwrite(u64 key, u64 type);
//
void waitkbd(u64* key, u64* type);
void main();




void start()
{
	//0.清空日志内存
	u64* p=(u64*)(journalhome);
	u64 type,key;
	for(key=0;key<0x20000;key++) p[key]=0;
	diary("oh we have found a whole new world,landing...%x...%x...%x...",3,2,1);

	//libhard
	initahci();
	initide();
	initxhci();

	//libsoft
	initacpi();
	//initparttable();
	//initfilesystem();

	//libui
	initscreen(window);
	initcharacter(window);

	//go
	main();




	//
	while(1)
	{
		characterread();
		writescreen();

		waitkbd(&key, &type);
		characterwrite(key, type);
	}




	//after
	__asm__("int $0x3");
}

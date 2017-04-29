#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




//libui1
void characterread();
void characterwrite(u64 key, u64 type);
//libui0
void writescreen();
//
void waitkbd(u64* key, u64* type);
//
void diary(char*,...);
void initconsole();




void main()
{
	u64 type,key;
	while(1)
	{
		characterread();
		writescreen();

		waitkbd(&key, &type);
		characterwrite(key, type);
	}
}

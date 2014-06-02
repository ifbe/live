#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


void mount()
{
	if( *(WORD*)0x12000b !=0x200) {
		say("not 512B per sector,bye!",0);
		return;
	}
	if( *(BYTE*)0x120010 !=2) {
		say("not 2 fat,bye!",0);
		return;
	}

	int similarity=50;

	if( *(WORD*)0x120011 ==0) similarity++;		//fat32为0
	else similarity--;
	if( *(WORD*)0x120016 ==0) similarity++;		//fat32为0
	else similarity--;

	if(similarity==48)
	{
		say("fat16",0);
		fat16();
		return;
	}
	if(similarity==52)
	{
		say("fat32",0);
		fat32();
		return;
	}
	say("seem not fatxx,bye!",0);
	return;

}

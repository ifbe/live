#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


int mountfat(QWORD sector)
{
	read(0x130000,sector,getdisk(),1); //pbr

	if( *(WORD*)0x13000b !=0x200) {
		say("not 512B per sector,bye!",0);
		return -1;
	}
	if( *(BYTE*)0x130010 !=2) {
		say("not 2 fat,bye!",0);
		return -2;
	}

	int similarity=50;

	if( *(WORD*)0x130011 ==0) similarity++;		//fat32为0
	else similarity--;
	if( *(WORD*)0x130016 ==0) similarity++;		//fat32为0
	else similarity--;

	if(similarity==48)
	{
		say("fat16",0);
		fat16();
	}
	else if(similarity==52)
	{
		say("fat32",0);
		fat32();
	}
	else
	{
		say("seem not fatxx,bye!",0);
		return -3;
	}

	unsigned int* pointer=(unsigned int*)0x180000;
	int i=0;
	for(i=0;i<0x1000;i+=4)
	{
		if( pointer[i] == 0x4556494c )
		{
			say("find our directory!!!!",0);
			return 1;
		}
	}

	say("not in this partition",0);
	return -4;
}

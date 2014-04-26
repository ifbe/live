#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


QWORD searchgpt()
{
	QWORD offset;
	for(offset=0x80000;offset<0x84000;offset+=0x80)    //find esp
	{
		if( *(QWORD*)offset==0x11d2f81fc12a7328 ){
			if( *(QWORD*)(offset+8)==0x3bc93ec9a0004bba ){
				offset=*(QWORD*)(offset+0x20);
				say("esp sector:",offset);
				return offset;
			}
		}
	}
	if(offset==0x84000)
	{
		say("no esp,bye!",0);
		return 0;
	}
}


QWORD searchmbr()
{
	QWORD offset;
	for(offset=0x801be;offset<0x801fe;offset+=0x10)    //find fat?
	{
		if( *(BYTE*)(offset+4)==0x7 | *(BYTE*)(offset+4)==0xb )
		{
			offset=(QWORD)(*(DWORD*)(offset+8));
			say("fat? sector:",offset);
			return offset;
		}

	}
	if(offset==0x801fe)
	{
		say("no fat,bye!",0);
		return 0;
	}
}


void parttable()
{
        read(0x80000,0,disk(),2);
        if( *(WORD*)0x801fe !=0xAA55 ){say("bad disk",0);return;}
        else say("good disk",0x55aa);

	QWORD fat;
	if(*(QWORD*)0x80200==0x5452415020494645){
		read(0x80000,2,disk(),32);
		fat=searchgpt();
		say("gpt part",fat);
	}
	else{
		fat=searchmbr();
		say("mbr disk",fat);
	}

	if(fat!=0) read(0x80000,fat,disk(),1); //pbr
	else say("no fat? partition",0);

}


void mount()
{
	if( *(WORD*)0x8000b !=0x200) {say("not 512B per sector,bye!",0);return;}
	if( *(BYTE*)0x80010 !=2) {say("not 2 fat,bye!",0);return;}

	int similarity=50;

	if( *(WORD*)0x80011 ==0) similarity++;		//fat32为0
	else similarity--;
	if( *(WORD*)0x80016 ==0) similarity++;		//fat32为0
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
	say("seem not fatx,bye!",0);
	return;

}

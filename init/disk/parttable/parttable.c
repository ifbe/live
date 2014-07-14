#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


QWORD explaingpt()
{
	say("gpt disk",0);
	BYTE* memory=(BYTE*)0x128000;
	int i;
	for(i=0;i<0x8000;i++) memory[i]=0;

	QWORD offset;
	BYTE* addr=(BYTE*)0x128000;
	for(offset=0x120400;offset<0x124400;offset+=0x80)    //find esp
	{
		if( *(QWORD*)offset==0x4433b9e5ebd0a0a2 ){
		if( *(QWORD*)(offset+8)==0xc79926b7b668c087 ){
			*(QWORD*)addr=0x7366746e;
			*(QWORD*)(addr+8)=*(QWORD*)(offset+0x20);
			addr+=0x10;
		}
		}
		else if( *(QWORD*)offset==0x11d2f81fc12a7328 ){
		if( *(QWORD*)(offset+8)==0x3bc93ec9a0004bba ){
			*(QWORD*)addr=0x746166;
			*(QWORD*)(addr+8)=*(QWORD*)(offset+0x20);
			addr+=0x10;
		}
		}
		else if( *(QWORD*)offset==0x477284830fc63daf ){
		if( *(QWORD*)(offset+8)==0xe47d47d8693d798e ){
			*(QWORD*)addr=0x747865;
			*(QWORD*)(addr+8)=*(QWORD*)(offset+0x20);
			addr+=0x10;
		}
		}
	}
}


QWORD explainmbr()
{
	say("mbr disk",0);
	BYTE* memory=(BYTE*)0x128000;
	int i;
	for(i=0;i<0x8000;i++) memory[i]=0;

	QWORD offset;
	BYTE* addr=(BYTE*)0x128000;
	for(offset=0x1201be;offset<0x1201fe;offset+=0x10)    //find fat?
	{
		BYTE temp=*(BYTE*)(offset+4);
		if( temp==0x4 | temp==0x6 | temp==0xb )
		{
			*(QWORD*)addr=0x746166;
			*(QWORD*)(addr+8)=(QWORD)(*(DWORD*)(offset+8));
			addr+=0x10;
		}
		else if( temp==0x7 )
		{
			*(QWORD*)addr=0x7366746e;
			*(QWORD*)(addr+8)=(QWORD)(*(DWORD*)(offset+8));
			addr+=0x10;
		}
		else if( temp==0x83 )
		{
			*(QWORD*)addr=0x747865;
			*(QWORD*)(addr+8)=(QWORD)(*(DWORD*)(offset+8));
			addr+=0x10;
		}
	}
}


void parttable()
{
	BYTE* memory=(BYTE*)0x120000;
	int i;
	for(i=0;i<0x8000;i++) memory[i]=0;

        read(0x120000,0,getdisk(),64);
        if( *(WORD*)0x1201fe !=0xAA55 ){say("bad disk",0);return;}

	if(*(QWORD*)0x120200==0x5452415020494645) explaingpt();
	else explainmbr();

	mounter();
}


void mount(QWORD name)
{
	blank2zero(&name);

	QWORD* memory=(QWORD*)0x128000;
	QWORD offset=0;
	int i;
	for(i=0;i<0x100;i+=2)
	{
		if(memory[i] == name)
		{
			offset=memory[i+1];
			break;	
		}
	}
	if(i==0x100)
	{
		say("partition not found",0);
		return;
	}

	if(name == 0x746166) mountfat(offset);
	if(name == 0x7366746e) mountntfs(offset);
	if(name == 0x747865) mountext(offset);
}

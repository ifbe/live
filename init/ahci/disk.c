#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


QWORD searchgpt()
{
	QWORD offset;
	for(offset=0x120000;offset<0x140000;offset+=0x80)    //find esp
	{
		if( *(QWORD*)offset==0x11d2f81fc12a7328 ){
			if( *(QWORD*)(offset+8)==0x3bc93ec9a0004bba ){
				offset=*(QWORD*)(offset+0x20);
				say("esp sector:",offset);
				return offset;
			}
		}
	}
	if(offset==0x140000)
	{
		say("no esp,bye!",0);
		return 0;
	}
}


QWORD searchmbr()
{
	QWORD offset;
	for(offset=0x1201be;offset<0x1201fe;offset+=0x10)    //find fat?
	{
		BYTE temp=*(BYTE*)(offset+4);
		if( temp==0x4 | temp==0xb | temp==0xb)
		{
			offset=(QWORD)(*(DWORD*)(offset+8));
			say("fat? sector:",offset);
			return offset;
		}

	}
	if(offset==0x1201fe)
	{
		say("no fat,bye!",0);
		return 0;
	}
}

void maybetesting()
{
	read(0x200000,2048,getdisk(),2048); //pbr
	say("get [2m,3m)",0);
}

void parttable()
{
        read(0x120000,0,getdisk(),2);
        if( *(WORD*)0x1201fe !=0xAA55 ){say("bad disk",0);return;}
        else say("good disk",0x55aa);

	QWORD fatxx;
	if(*(QWORD*)0x120200==0x5452415020494645){
		read(0x120000,2,getdisk(),32);
		fatxx=searchgpt();
		say("gpt part",fatxx);
	}
	else{
		fatxx=searchmbr();
		say("mbr disk",fatxx);
	}

	if(fatxx!=0){
		read(0x120000,fatxx,getdisk(),1); //pbr
		say("get pbr",0);
	}
	else{
		maybetesting();
	}

}

void mount()
{
	if( *(WORD*)0x12000b !=0x200) {
		say("not 512B per sector,bye!",0);
		maybetesting();
		return;
	}
	if( *(BYTE*)0x120010 !=2) {
		say("not 2 fat,bye!",0);
		maybetesting();
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

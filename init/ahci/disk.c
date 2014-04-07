#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


void readtable()
{
        read(0x100000,0,disk(),2);
        if( *(WORD*)0x1001fe !=0xAA55 ){say("bad disk",0);return;}
        else say("good disk",0);
}


QWORD searchgpt()
{
	QWORD offset;
	for(offset=0x100000;offset<0x104000;offset+=0x80)    //find esp
	{
		if( *(QWORD*)offset==0x11d2f81fc12a7328 ){
			if( *(QWORD*)(offset+8)==0x3bc93ec9a0004bba ){
				offset=*(QWORD*)(offset+0x20);
				say("findesp:",offset);
				return offset;
			}
		}
	}
	if(offset==0x104000)
	{
		say("no esp,bye!",0);
		return 0;
	}
}


QWORD searchmbr()
{
	QWORD offset;
	for(offset=0x1001be;offset<0x1001fe;offset+=0x10)    //find fat?
	{
		if( *(BYTE*)(offset+4)==0x7 | *(BYTE*)(offset+4)==0xb )
		{
			offset=(QWORD)(*(DWORD*)(offset+8));
			say("findfatx:",offset);
			return offset;
		}

	}
	if(offset==0x1000fe)
	{
		say("no fat,bye!",0);
		return 0;
	}
}


void readpart()
{
	QWORD fat;
	if(*(QWORD*)0x100200==0x5452415020494645){
		say("gpt part",0);
		read(0x100000,2,disk(),32);
		fat=searchgpt();
	}
	else{
		say("mbr disk",0);
		fat=searchmbr();
	}

	if(fat!=0)
	{
        	read(0x100000,fat,disk(),1); //pbr
		say("pbr:",fat);
	}
	else say("no our partition",0);

}


void automount()
{
	if( *(WORD*)0x10000b !=0x200) {say("not 512B sector,bye!",0);return;}
	if( *(BYTE*)0x100010 !=2) {say("not 2 fat,bye!",0);return;}

	int similarity=50;

	if( *(WORD*)0x100011 ==0) similarity++;		//fat32为0
	else similarity--;
	if( *(WORD*)0x100016 ==0) similarity++;		//fat32为0
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

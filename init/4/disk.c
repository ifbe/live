#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

QWORD finddisk()
{
        QWORD disk,addr=0x2808;

        for(;addr<0x3000;addr+=0x10)
        {
                if( *(unsigned int*)addr ==0x61746173) break;
        }

	if(addr==0x3000) return 0;

        disk=(QWORD)(*(unsigned int*)(addr-8));
	return disk;
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
			say("find0x07or0x0b:",offset);
			return offset;
		}

	}
	if(offset==0x1000fe)
	{
		say("no fat,bye!",0);
		return 0;
	}
}


QWORD findfat(QWORD disk)
{
	QWORD fat;
	if(*(QWORD*)0x100200==0x5452415020494645){
		say("gpt part",0);
		read(0x100000,2,disk,32);
		fat=searchgpt();
	}
	else{
		say("mbr disk",0);
		fat=searchmbr();
	}
	return fat;
}


QWORD fstype()
{
	if( *(WORD*)0x10000b !=0x200) {say("not 512B sector,bye!",0);return;}
	if( *(BYTE*)0x100010 !=2) {say("not 2 fat,bye!",0);return;}

	int similarity=50;

	if( *(WORD*)0x100011 ==0) similarity++;		//fat32为0
	else similarity--;
	if( *(WORD*)0x100016 ==0) similarity++;		//fat32为0
	else similarity--;

	if(similarity==48){say("fat16",0);return 16;}
	if(similarity==52){say("fat32",0);return 32;}
	say("strange partition,bye!",0);
	return 0;
}

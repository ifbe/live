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
		if( temp==0x4 | temp==0x6 | temp==0xb)
		{
			offset=(QWORD)(*(DWORD*)(offset+8));
			return offset;
		}

	}
	if(offset==0x1201fe)
	{
		say("no fat,bye!",0);
		return 0;
	}
}

void parttable()
{
        read(0x120000,0,getdisk(),2);
        if( *(WORD*)0x1201fe !=0xAA55 ){say("bad disk",0);return;}
        else say("good disk",0);

	QWORD fatxx;
	if(*(QWORD*)0x120200==0x5452415020494645){
		read(0x120000,2,getdisk(),32);
		fatxx=searchgpt();
		say("gpt disk",0);
	}
	else{
		fatxx=searchmbr();
		say("mbr disk",0);
	}

	if(fatxx!=0){
		read(0x120000,fatxx,getdisk(),1); //pbr
		say("partition sector:",fatxx);
	}
}

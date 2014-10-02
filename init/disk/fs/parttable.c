#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define diskhome 0x400000
#define mbrbuffer diskhome+0x20000




//从磁盘读出来的数据在[+0x400,+0x4400]=0x80个*每个0x80
//[+0,+0xf]:类型guid			raw[0],raw[1]
//[+0x10,+0x1f]:分区guid		raw[2],raw[3]
//[+0x20,+0x27]:起始lba			raw[4]
//[+0x28,+0x2f]:末尾lba			raw[5]
//[+0x30,+0x37]:属性标签		raw[6]
//[+0x38,+0x7f]:名字			raw[7]~raw[0xf]
//翻译之后的在[diskhome,diskhome+0x800]
//[+0,0x7]:起始lba			our[0]
//[+0x8,0xf]:末尾lba			our[1]
//[+0x10,0x17]:分区类型anscii		our[2]
//[0x18,0x3f]:空
QWORD explaingpt()
{
	say("gpt disk",0);
	QWORD* our=(QWORD*)diskhome;
	QWORD* raw=(QWORD*)(mbrbuffer+0x400);
	int i;
	for(i=0;i<0x200;i++) our[i]=0;

	for(i=0;i<0x80;i++)
	{
		our[8*i]=raw[0x10*i+4];
		our[8*i+1]=raw[0x10*i+5];
		if(raw[0x10*i]==0x4433b9e5ebd0a0a2)
		{
			if(raw[0x10*i+1]==0xc79926b7b668c087)
				our[8*i+2]=0x7366746e;
		}
		else if(raw[0x10*i]==0x11d2f81fc12a7328)
		{
			if(raw[0x10*i+1]==0x3bc93ec9a0004bba)
				our[8*i+2]=0x746166;
		}
		else if(raw[0x10*i]==0x477284830fc63daf)
		{
			if(raw[0x10*i+1]==0xe47d47d8693d798e)
				our[8*i+2]=0x747865;
		}
	}
}




//从磁盘读出来的数据在[+0,+0xf]=4个*每个0x10
//[+0]:活动标记
//[+0x1,+0x3]:开始磁头柱面扇区
//[+0x4]:分区类型
//[+0x5,+0x7]:结束磁头柱面扇区
//[+0x8,+0xb]:起始lba
//[+0xc,+0xf]:大小
//翻译之后的在[diskhome,diskhome+0x800]
//[+0,0x7]:起始lba			our[0]
//[+0x8,0xf]:末尾lba			our[1]
//[+0x10,0x17]:分区类型anscii		our[2]
//[0x18,0x3f]:空
QWORD explainmbr()
{
	say("mbr disk",0);
	QWORD raw=mbrbuffer+0x1be;
	BYTE* our=(BYTE*)diskhome;
	int i;
	for(i=0;i<0x1000;i++) our[i]=0;

	for(i=0;i<4;i++)
	{
		*(QWORD*)(our+0x40*i)=*(DWORD*)(raw+0x10*i+8);
		*(QWORD*)(our+0x40*i+8)=*(DWORD*)(raw+0x10*i+0xc);


		BYTE temp=*(BYTE*)(raw+0x10*i+4);
		if( temp==0x4 | temp==0x6 | temp==0xb )
		{
			*(QWORD*)(our+0x40*i+0x10)=0x746166;
		}
		else if( temp==0x7 )
		{
			*(QWORD*)(our+0x40*i+0x10)=0x7366746e;
		}
		else if( temp==0x83 )
		{
			*(QWORD*)(our+0x40*i+0x10)=0x747865;
		}
	}
}


void remember(QWORD cd,QWORD load)
{
	QWORD* pointer=(QWORD*)0x4000;
	int i;
	for(i=0;i<0x200;i+=2)
	{
	if( (pointer[i]==0) | (pointer[i]==0x6463) )
	{
		pointer[i]=0x6463;
		pointer[i+1]=cd;
		pointer[i+2]=0x64616f6c;
		pointer[i+3]=load;
		break;
	}
	}
}


static void mount(QWORD name)
{
	blank2zero(&name);

	QWORD* memory=(QWORD*)diskhome;
	QWORD offset=0;
	int i;
	for(i=0;i<0x80;i+=8)
	{
		if(memory[i+2] == name)
		{
			offset=memory[i];
			break;	
		}
	}
	if(i>=0x80)
	{
		say("partition not found",0);
		return;
	}

	if(name == 0x746166) mountfat(offset);
	if(name == 0x7366746e) mountntfs(offset);
	if(name == 0x747865) mountext(offset);
}


void parttable()
{
	BYTE* memory=(BYTE*)(mbrbuffer);
	int i;
	for(i=0;i<0x8000;i++) memory[i]=0;

        read(mbrbuffer,0,getdisk(),64);
        if(*(WORD*)(mbrbuffer+0x1fe)!=0xAA55){say("bad disk",0);return;}

	if(*(QWORD*)(mbrbuffer+0x200)==0x5452415020494645) explaingpt();
	else explainmbr();




	QWORD* pointer=(QWORD*)0x4000;
	for(i=0;i<0x200;i+=2)
	{
	if( (pointer[i]==0) | (pointer[i]==0x746e756f6d) )
	{
		pointer[i]=0x746e756f6d;		//"mount"
		pointer[i+1]=(unsigned long long)mount;
		break;
	}
	}
}

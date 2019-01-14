#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long




void say(char* , ...);








//从磁盘读出来的数据在[+0x400,+0x4400]=0x80个*每个0x80
//[+0,+0xf]:类型guid			source[0],source[1]
//[+0x10,+0x1f]:分区guid		source[2],source[3]
//[+0x20,+0x27]:起始lba			source[4]
//[+0x28,+0x2f]:末尾lba			source[5]
//[+0x30,+0x37]:属性标签		source[6]
//[+0x38,+0x7f]:名字			source[7]~source[0xf]
//翻译之后的在[diskhome,diskhome+0x800]
//[+0,0x7]:起始lba			dest[0]
//[+0x8,0xf]:末尾lba			dest[1]
//[+0x10,0x17]:分区类型anscii		dest[2]
//[0x18,0x3f]:空
void explaingpt(QWORD* source,QWORD* dest)
{
	int i;
	for(i=0;i<0x200;i++) dest[i]=0;

	say("gpt disk");
	for(i=0;i<0x80;i++)
	{
		dest[8*i]=source[0x10*i+4];
		dest[8*i+1]=source[0x10*i+5];
		if(source[0x10*i]==0x4433b9e5ebd0a0a2)
		{
			if(source[0x10*i+1]==0xc79926b7b668c087)
				dest[8*i+2]=0x7366746e;
		}
		else if(source[0x10*i]==0x11d2f81fc12a7328)
		{
			if(source[0x10*i+1]==0x3bc93ec9a0004bba)
				dest[8*i+2]=0x746166;
		}
		else if(source[0x10*i]==0x477284830fc63daf)
		{
			if(source[0x10*i+1]==0xe47d47d8693d798e)
				dest[8*i+2]=0x747865;
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
//[+0,0x7]:起始lba			dest[0]
//[+0x8,0xf]:末尾lba			dest[1]
//[+0x10,0x17]:分区类型anscii		dest[2]
//[0x18,0x3f]:空
void explainmbr(QWORD source,QWORD dest)
{
	char* p=(char*)dest;
	int i;
	for(i=0;i<0x1000;i++) p[i]=0;

	say("mbr disk");
	for(i=0;i<4;i++)
	{
		*(QWORD*)(dest+0x40*i)=*(DWORD*)(source+0x10*i+8);
		*(QWORD*)(dest+0x40*i+8)=*(DWORD*)(source+0x10*i+0xc);

		BYTE temp=*(BYTE*)(source+0x10*i+4);
		if( temp==0x4 | temp==0x6 | temp==0xb )
		{
			*(QWORD*)(dest+0x40*i+0x10)=0x746166;
		}
		else if( temp==0x7 )
		{
			*(QWORD*)(dest+0x40*i+0x10)=0x7366746e;
		}
		else if( temp==0x83 )
		{
			*(QWORD*)(dest+0x40*i+0x10)=0x747865;
		}
	}
}
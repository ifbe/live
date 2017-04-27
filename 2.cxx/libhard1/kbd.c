#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
u8 ps2kbd();
void diary(char*,...);




static u8 kbd[5*2] =
{
	0x48,0x48,	//up
	0x50,0x50,	//down
	0x4b,0x4b,	//left
	0x4d,0x4d,	//right
	0x1,0x1b	//esc
};
static u8 ch[30*2] =
{
        0x1e,'a',
        0x30,'b',
        0x2e,'c',
        0x20,'d',
        0x12,'e',
        0x21,'f',
        0x22,'g',
        0x23,'h',
        0x17,'i',
        0x24,'j',
        0x25,'k',
        0x26,'l',
        0x32,'m',
        0x31,'n',
        0x18,'o',
        0x19,'p',
        0x10,'q',
        0x13,'r',
        0x1f,'s',
        0x14,'t',
        0x16,'u',
        0x2f,'v',
        0x11,'w',
        0x2d,'x',
        0x15,'y',
        0x2c,'z',
        0x0e,0x7f,      //backspace
        0x1c,0xd,       //enter
        0x39,0x20,      //space
	0x00,0x00
};




int waitkbd(u64* key, u64* type)
{
	int j;
	*type = 0;

	//key down
	while(1)
	{
		*key = ps2kbd();
		if(*key < 0x80)break;
	}

	//kbd
	if(*type==0)
	{
		for(j=0;j<5;j++)
		{
			if(*key == kbd[j*2])
			{
				*type = 0x64626b;
				*key = kbd[(j*2) + 1];
				break;
			}
		}
	}

	//char
	if(*type==0)
	{
		for(j=0;j<30;j++)
		{
			if(*key == ch[j*2])
			{
				*type = 0x72616863;
				*key = ch[(j*2) + 1];
				break;
			}
		}
	}

	//return
	//diary("type=%x,key=%x",*type,*key);
}

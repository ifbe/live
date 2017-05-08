#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int readkbd(u8*, int);
void say(char*,...);




static u8 kbd[9*2] =
{
	0x1,0x1b,	//esc
	0x47,0x47,	//home
	0x4f,0x4f,	//end
	0x49,0x49,	//page up
	0x51,0x51,	//page down
	0x48,0x48,	//up
	0x50,0x50,	//down
	0x4b,0x4b,	//left
	0x4d,0x4d	//right
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




struct event
{
	u64 why;
	u64 what;
	u64 where;
	u64 when;
};
int waitkbd(struct event* ev)
{
	int j;
	u8 buf[1];
	j = readkbd(buf, 1);
	if(j == 0)return 0;
	if(buf[0] >= 0x80)return 0;

	//kbd
	ev->what = 0;
	ev->why = buf[0];
	if(ev->what==0)
	{
		for(j=0;j<9;j++)
		{
			if(ev->why == kbd[j*2])
			{
				ev->what = 0x64626b;
				ev->why = kbd[(j*2) + 1];
				break;
			}
		}
	}

	//char
	if(ev->what==0)
	{
		for(j=0;j<30;j++)
		{
			if(ev->why == ch[j*2])
			{
				ev->what = 0x72616863;
				ev->why = ch[(j*2) + 1];
				break;
			}
		}
	}

	//say("type=%x,key=%x",*type,*key);
	return 1;
}

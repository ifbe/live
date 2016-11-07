#define u8 unsigned char
u8 ps2kbd();
void diary(char*,...);




static u8 table[30*2] =
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




int waitkbd()
{
	int j;
	u8 key = ps2kbd();
	for(j=0;j<30;j++)
	{
		if(table[j*2] == key)
		{
			key = table[(j*2) + 1];
			break;
		}
	}
	diary("key=%x",key);
	return key;
}

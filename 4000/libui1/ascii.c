#define u64 unsigned long long
#define u32 unsigned int
#define slowanscii 0x3000
#define fastanscii 0x30000
#define window 0x1000000




void printascii(int xxxx, int yyyy, int size, char ch, u32 fgcolor, u32 bgcolor)
{
	int x,y,j,k,flag;
	int width,height;
	unsigned char temp;
	unsigned char* points;
	u32* screen;

	if(ch<0x20)ch=0x20;
	points=(unsigned char*)slowanscii;
	points+=ch<<4;

	size &= 0x7;
	if(size == 0)size=1;

	flag = bgcolor>>24;
	fgcolor |= 0xff000000;
	bgcolor |= 0xff000000;

	width = 1024;
	height = 768;
	screen = (u32*)window;

	for(y=0;y<16;y++)
	{
		temp=points[0];
		points++;

		for(x=0;x<8;x++)
		{
			for(j=0;j<size;j++)
			{
				for(k=0;k<size;k++)
				{
					if( (temp&0x80) != 0 )
					{
						screen[ ( (yyyy+y*size+k) * width ) + (xxxx+x*size+j)] = fgcolor;
					}
					else if(flag != 0)
					{
						screen[ ( (yyyy+y*size+k) * width ) + (xxxx+x*size+j)] = bgcolor;
					}
				}//k
			}//j

			temp<<=1;
		}//x
	}//y
}
void printbyte(int x, int y, int size, unsigned char ch, u32 fg, u32 bg)
{
	int i;
	unsigned char temp=ch;

	ch=(temp>>4) & 0xf;
	ch+=0x30;
	if(ch>0x39)ch+=0x7;
	printascii(x, y, size, ch, fg, bg);

	ch=temp & 0xf;
	ch+=0x30;
	if(ch>0x39)ch+=0x7;
	printascii(x + size*8, y, size, ch, fg, bg);
}




void printstring(int x, int y, int size, char* p, u32 fgcolor, u32 bgcolor)
{
	int j=0;

	size &= 0x7;
	if(size==0)size=1;

	while(1)
	{
		if(*p == 0x00 )break;
		if( j >= 0x80 )break;

		printascii(x+j*size*8, y, size, *p, fgcolor, bgcolor);
		j++;
		p++;
	}
}




void printdecimal(int x, int y, int size, int dec, u32 fgcolor, u32 bgcolor)
{
	char ch;
	int i,count;
	long long temp;

	size &= 0x7;
	if(size==0)size=1;

	if(dec<0)
	{
		printascii(x, y, size, '-', fgcolor, bgcolor);
		x += size*8;
		dec=-dec;
	}

	count=0;
	temp=dec;
	while(1)
	{
		if(temp<10)break;
		temp=temp/10;
		count++;
	}

	for(i=0;i<=count;i++)
	{
		ch=(char)(dec%10);
		if(ch<=9)ch+=0x30;
		else if(ch<=0xf)ch+=0x37;
		printascii(x+(count-i)*size*8, y, size, ch, fgcolor, bgcolor);
		dec=dec/10;
	}
}




void printhexadecimal(int x, int y, int size, u64 hex, u32 fgcolor, u32 bgcolor)
{
	int i;
	char ch;
	unsigned long long temp;

	i=0;
	temp=hex;
	size &= 0x7;
	if(size == 0)size=1;

	while(1)
	{
		if(temp<0x10)break;
		temp=temp>>4;
		i++;
	}
	for(;i>=0;i--)
	{
		ch=(char)(hex&0x0000000f);
		if(ch<=9)ch+=0x30;
		else if(ch<=0xf)ch+=0x37;
		printascii(x+i*size*8, y, size, ch, fgcolor, bgcolor);
		hex=hex>>4;
	}
}
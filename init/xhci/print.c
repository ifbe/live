
void point(int x,int y,int color)
{
    unsigned long long* video=(unsigned long long*)0x3028;
    unsigned long long base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=color;
}
void anscii(int x,int y,char ch)
{
    int i,j;
    unsigned long long source=0x6000;
    char temp;
    char* p;

    if(ch<0)ch=0x20;
    source+=ch<<4;
    x=8*x;
    y=16*y;

    for(i=0;i<16;i++)
    {
        p=(char*)source;
        for(j=0;j<8;j++)
        {
            temp=*p;
            temp=temp<<j;
            temp&=0x80;
            if(temp!=0){point(j+x,i+y,0xffffffff);}
            else{point(j+x,i+y,0);}

        }
    source++;
    }
}
void say(char* p,...)
{
        register unsigned long long rdi asm("rdi");
        register unsigned long long rsi asm("rsi");
	unsigned long long first=rsi;
	unsigned long long x=0;
	unsigned long long y=where();

	while(*p!='\0')
	{
		anscii(x+64,y,*p);
		*(char*)(0x7800+y*64+x)=*p;
		p++;
		x++;
	}

	if(first!=0)
	{
	        int i=0;
		char ch;
		*(unsigned long long*)(0x7820+y*64)=first;
	        for(i=15;i>=0;i--)
	        {
	        ch=(char)(first&0xf);
	        first=first>>4;
	        anscii(x+64+i,y,ch);
	        }
	}

	if(y>=50) cleanwhere();
	else incwhere();
}

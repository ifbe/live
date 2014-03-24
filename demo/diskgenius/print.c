int where=0;

void point(int x,int y,int color)
{
    unsigned long long * video=(unsigned long long *)0x3028;
    unsigned long long  base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=color;
}
void anscii(int x,int y,char ch)
{
    int i,j;
    unsigned long long  points=0xb000;
    char temp;
    char* p;

    if(ch<0)ch=0x20;
    points+=ch<<4;
    x=8*x;
    y=16*y;

    for(i=0;i<16;i++)
    {
        p=(char*)points;
        for(j=0;j<8;j++)
        {
            temp=*p;
            temp=temp<<j;
            temp&=0x80;
            if(temp!=0){point(j+x,i+y,0xffffffff);}
            else{point(j+x,i+y,0);}

        }
    points++;
    }
}
void say(char* p,...)
{
        register unsigned long long rsi asm("rsi");
        register unsigned long long rdx asm("rdx");
        register unsigned long long rcx asm("rcx");
        register unsigned long long r8 asm("r8");
        register unsigned long long r9 asm("r9");
        unsigned long long first=rsi;

	int x=0;
	while(*p!='\0')
	{
		anscii(x,where,*p);
		p++;
		x++;
	}

        int i=0;
	char ch;
        for(i=15;i>=0;i--)
        {
        ch=(char)(first&0x0000000f);
        first=first>>4;
        anscii(x+i,where,ch);
        }

	where++;
	if(where==50)where=0;
}

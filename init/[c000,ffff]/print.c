#define QWORD unsigned long long
int where=0;


void point(int x,int y,int color)
{
    QWORD* video=(QWORD*)0x3028;
    QWORD base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=color;
}
void anscii(int x,int y,char ch)
{
    int i,j;
    QWORD points=0xb000;
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
void say(char* p,QWORD z)
{
	int x=0;
	while(*p!='\0')
	{
		anscii(x+64,where,*p);
		p++;
		x++;
	}

        int i=0;
	char ch;
        for(i=15;i>=0;i--)
        {
        ch=(char)(z&0x0000000f);
        z=z>>4;
        anscii(96+i,where,ch);
        }

	where++;
	if(where==50)where=0;
}

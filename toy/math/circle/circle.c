#define u64 long long
double cosine(double x);
double sine(double x);




static inline void outw( unsigned short port, unsigned short val )
{
    asm volatile( "outw %0, %1"
                  : : "a"(val), "Nd"(port) );
}


static inline unsigned char inb( unsigned short port )
{
    unsigned char ret;
    asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}


char keyboard()
{
    while((inb(0x64)&0x01)==0);         //阻塞
    return inb(0x60);
}


void turnoff()
{
    short* p;
    short port,val;
    p=(short*)0x4fc;
    port=*p;
    p=(short*)0x4fe;
    val=(*p)|0x2000;
    outw(port,val);
}


void main()
{
    double angle=3.1415*2;
    int x,y; 

    for(x=0;x<1024;x++)
    {
	for(y=0;y<768;y++)
	{
		point(x,y,0x44444444);
	}
    }

    while((angle-=0.001)>0)
    {
        x=(int)(250*cosine(angle));
        y=(int)(250*sine(angle));
        draw(x,y,0x000000);
    }

    while(keyboard()!=0x01);
    turnoff();
}

#define u64 long long
int x=-250,y=0,xx,yy;

void point(int x,int y,int z)
{
    u64* video=(u64*)0x3028;
    u64 base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=z;
}


void draw(int x,int y,int z)
{
    int i,j;
    for(i=-5;i<5;i++)
        for(j=-5;j<5;j++)
            point(x+512+i,384-y+j,z);
}

void move()
{
    draw(x,y,0xff00);
    x+=xx;
    y+=yy;
    if((y>360)|(y<-360))
    {
        yy=-yy;
    }
    if((x>-10)|(x<-500))
    {
        xx=-xx;
    }
    draw(x,y,0);
}

void main()
{
    int i;
    xx=(int)(*((char*)0x500)>>4)+8;
    yy=(int)(*((char*)0x500)&0x0f)+8;

    while(1)
    {
	for(i=0;i<8000000;i++);
	asm("hlt");
	move();
    }
}


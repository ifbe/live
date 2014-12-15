#define u64 long long
static int table[16][16];


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


void anscii(int x,int y,char ch)
{
    int i,j;
    u64 points=0x6000;
    char temp;
    char* p;

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


void hexadecimal(int x,int y,u64 z)
{
        char ch;
        int i=0;

        for(i=7;i>=0;i--)
        {
        ch=(char)(z&0x0000000f);
        z=z>>4;
        anscii(x+i,y,ch);
        }
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
    char ret,temp;
    do{
        while((inb(0x64)&0x01)==0);         //阻塞
        ret=inb(0x60);
        temp=ret&0x80;
    }while(temp!=0);
    return ret;
}


void main()
{
	int x=0,y=0,color,red=0,green=0,blue=0;
	char key=0;

	for(x=0;x<16;x++)
		for(y=0;y<16;y++)
			table[x][y]=0;

	while(1)
	{
		for(x=0;x<256;x++)
			for(y=0;y<256;y++)
			{
				color=red*65536+y*256+x;
				point(2*x,2*y,color);
				point(2*x,2*y+1,color);
				point(2*x+1,2*y,color);
				point(2*x+1,2*y+1,color);
			}

		color=red*65536+green*256+blue;
		for(x=512;x<1024;x++)
			for(y=0;y<512;y++)
				point(x,y,color);

		point(2*blue,2*green,0xffffffff);
		point(2*blue+1,2*green,0xffffffff);
		point(2*blue,2*green+1,0xffffffff);
		point(2*blue+1,2*green+1,0xffffffff);
		hexadecimal(60,0,red*65536+green*256+blue);

		key=keyboard();
		if(key==0x1){break;}
		if(key==0x4a){if(red>0) red--;}	//left
		if(key==0x4e){if(red<255) red++;}	//right
		if(key==0x48){if(green>0) green--;}	//up
		if(key==0x50){if(green<255) green++;}	//down
		if(key==0x4b){if(blue>0) blue--;}	//left
		if(key==0x4d){if(blue<255) blue++;}	//right
	}
}

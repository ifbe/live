#define u64 long long


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


void cubie(x,y,z)
{
	int i,j;
	x=40*x;
	y=40*y;
	for(i=0;i<40;i++)
		for(j=0;j<40;j++)
			point(x+i,y+j,z);
}


void anscii(int x,int y,char ch)
{
    int i,j;
    u64 points=0x7000;
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

        for(i=3;i>=0;i--)
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
	int i,j,x=0,y=0;
	int table[16][16];
	char key=0;

	for(i=0;i<16;i++)
		for(j=0;j<16;j++)
			table[i][j]=0;

	while(1)
	{
		for(i=0;i<16;i++)
			for(j=0;j<16;j++)
				{
					cubie(i,j,table[i][j]);
					point(i+720,j+320,table[i][j]);
				}
		for(j=40;j<=640;j+=40)			//heng
			for(i=0;i<640;i++)
				point(i,j,0xffffffff);
		for(i=40;i<=640;i+=40)			//shu
			for(j=0;j<=640;j++)
				point(i,j,0xffffffff);
		for(i=40*y;i<40*(y+1);i++)
		{
			point(40*x,i,0xff0000);
			point(40*(x+1),i,0xff0000);
		}
		for(i=40*x;i<40*(x+1);i++)
		{
			point(i,40*y,0xff0000);
			point(i,40*(y+1),0xff0000);
		}

		int theword=0;
		for(j=0;j<16;j++)
		{
			theword=0;
			for(i=0;i<16;i++)
			{
				theword*=2;
				if(table[i][j]<0){theword++;}
			}
			hexadecimal(100,2*j,theword);
		}

		key=keyboard();
		if(key==0x1){break;}
		if(key==0x4b){if(x>0)	x--;}	//left
		if(key==0x4d){if(x<15)	x++;}	//right
		if(key==0x48){if(y>0)	y--;}	//up
		if(key==0x50){if(y<15)	y++;}	//down
		if(key==0x39){table[x][y]=~table[x][y];}	//space
	}
}

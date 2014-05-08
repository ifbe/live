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


void decimal(int x,int y,u64 z)
{
        char ch;
        int i=0;
        u64 temp;

        if(z<0){
        anscii(x,y,'-');
        x++;
        z=-z;
        }

        temp=z;
        while(temp>=10){
        temp=temp/10;
        i++;
        }

        for(;i>=0;i--)
        {
        ch=(char)(z%10);
        z=z/10;
        anscii(x+i,y,ch);
        }
}



void value(x,y,z)
{
	decimal(8+x*20,4+y*10,z);
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
	int i,j;
	int table[4][4];
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			table[i][j]=1087;

	char key=0;
	while(1)
	{
		for(i=0;i<640;i++)
			for(j=0;j<640;j++)
				point(i,j,0xeeaa55be);
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				value(i,j,table[i][j]);

		for(j=160;j<=640;j+=160)		//heng
			for(i=0;i<640;i++)
				point(i,j,0xffffffff);
		for(i=160;i<=640;i+=160)		//shu
			for(j=0;j<=640;j++)
				point(i,j,0xffffffff);

		key=keyboard();
		if(key==0x1){break;}
		if(key==0x4b){if(i>0)	i--;}	//left
		if(key==0x4d){if(i<15)	i++;}	//right
		if(key==0x48){if(j>0)	j--;}	//up
		if(key==0x50){if(j<15)	j++;}	//down
		if(key==0x39){table[i][j]=~table[i][j];}	//space
	}
}

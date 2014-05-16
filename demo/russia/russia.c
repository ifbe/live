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



void cubie(int x,int y,int z)
{
	int i,j;

	for(i=x*40;i<(x+1)*40;i++){
	for(j=40*y;j<(y+1)*40;j++){
		point(i,j,z>0?0xffffffff:0);
	}
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


int random()
{
        int key,i=0;
        char* memory=(char*)0x0;
        for(i=0;i<0x1000;i++)
                key+=memory[i];
        return key;
}


int power(int in)
{
	if(in==0) return 1;

	int temp=2;
	for(in-=1;in>0;in--) temp*=2;
	return temp;
}


void main()
{
	int i,j,temp;
	char table[16][8];

	for(i=0;i<4;i++){
	for(j=0;j<4;j++){
		//temp=j*4 +i;
		//table[i][j]=power(temp);
		table[i][j]=0;
	}
	}

	char key=0;
	while(1)
	{
		table[0][0]=power(random() & 0x1);
		cubie(0,0,table[0][0]);
		key=keyboard();
	}
}

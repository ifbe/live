#define u64 long long
void point(int x,int y,int z);
void print(int x,int y,char ch);
void print32(int x,int y,int z);

void start()
{
    int i=0x12345678;
    print32(0,0,i);

hlt:asm("hlt");
goto hlt;
}


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


void print(int x,int y,char ch)
{
    int i,j;
    u64 rsi=0xb000;
    char temp;
    char* p;

    rsi+=ch<<4;
    x=8*x;
    y=16*y;

    for(i=0;i<16;i++)
    {
        p=(char*)rsi;
        for(j=0;j<8;j++)
        {
            temp=*p;
            temp=temp<<j;
            temp&=0x80;
            if(temp!=0){point(j+x,i+y,0);}
            else{point(j+x,i+y,0x00ffff00);}

        }
    rsi++;
    }
}

void print32(int x,int y,int z)
{
    char ch;
    int i;

    for(i=7;i>=0;i--)
    {
        ch=(char)(z&0x0000000f);
        z=z>>4;
        print(x+i,y,ch);
    }
}

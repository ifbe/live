#define u64 unsigned long long


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
    u64 rsi=0x4000;
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
            if(temp!=0){point(j+x,i+y,0xffffffff);}
            else{point(j+x,i+y,0x44444444);}
        }
    rsi++;
    }
}


void string(int x,int y,char* p)
{
    while(*p!='\0')
    {
    print(x,y,*p);
    p++;
    x++;
    }
}


void draw(int x,int y,int z)
{
    point(x+512,384-y,z);
}


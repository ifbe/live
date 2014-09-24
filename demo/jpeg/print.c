void point(int x,int y,int z)
{
    long long* video=(long long*)0x3028;
    long long base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=z;
}


void print(int x,int y,char ch)
{
    int i,j;
    long long rsi=0x5000;
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

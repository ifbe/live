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


void decimal(int x,int y,u64 z)
{
        char ch;
        int i=0;
        u64 temp;

        if(z<0){
        print(x,y,'-');
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
        print(x+i,y,ch);
        }
}


void printdouble(int x,int y,double value)
{
    u64 temp;
    int i;
    if(value<0)
    {
        value=-value;
        print(x,y,'-');
    }
    temp=(u64)value;
    value=(value-(double)temp);      //清空小数点前面的
    value*=100000000;
    for(i=1;i<9;i++)
    {
        print(x+9-i,y,temp%10+0x30);
        temp/=10;
    }
    print(x+9,y,'.');
    temp=(u64)value;
    for(i=1;i<9;i++)
    {
        print(x+18-i,y,temp%10+0x30);
        temp/=10;
    }
}


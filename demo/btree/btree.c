#define u64 long long


void init()
{
    u64* video=(u64*)0x3028;
    u64 base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;
    int i;

    for(i=0;i<1024*768;i++)    //1024*768
    {
        video=(u64*)base;
        *video=0;
        base+=bpp;
    }
}


char convert(char old)
{
    char* p;
    u64 rsi;
    char new=0x20;
    int i;

    rsi=0x6800;
    for(i=0;i<44;i++)
    {
        p=(char*)rsi;
        if(*p==old)
        {
            rsi++;
            p=(char*)rsi;
            new=*p;
            break;
        }
        rsi+=2;
    }
    return new;
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
    u64 rsi=0x6000;
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
            else{point(j+x,i+y,0);}
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


void main()
{
    char buffer[128];
    char anscii[128];
    int i;
    anscii[127]='\0';        //字符串结尾
    for(i=0;i<127;i++){anscii[i]=0x20;}
    i=0;

    init();
    string(55,20,"b+ tree!!!!");

    while(1)
    {
        buffer[i]=hltwait();
	if(buffer[i]>=0x80) continue;

        switch(buffer[i])
        {
            case 0x01: break;
            case 0x0e:{
                print(i-1,0,0x20);
                anscii[i]=0x20;
                if(i!=0)i--;
                break;
            }
            case 0x1c:{
                //analyse(anscii);           //进行计算
                for(i=0;i<127;i++){anscii[i]=0x20;} //处理完毕，清空
                i=0;                           //从头再来
                break;
            }
            default:{
                anscii[i]=convert(buffer[i]);  //进来一个convert一个
                string(0,0,anscii);
                i=(i+1)%127;
            }
        }
    }
}

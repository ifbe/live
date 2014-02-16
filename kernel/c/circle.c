#define u64 long long
static inline unsigned char inb(unsigned short port);
static inline void outw(unsigned short port,unsigned short val);
char keyboard();
void turnoff();
void init();
void point(int x,int y,int z);
void draw(int x,int y,int z);
void circle();
double absolute(double ret);
double squareroot(double x);
double ln(double a);
double logarithm(double x,double y);
double exponent(double x);
double power(double x,double y);
double cosine(double x);
double sine(double x);


void start()                //位置固定在第一个函数
{
    init();               //清屏
    circle();               //太极

    while(keyboard()!=0x01);
    turnoff();
}


static inline void outw( unsigned short port, unsigned short val )
{
    asm volatile( "outw %0, %1"
                  : : "a"(val), "Nd"(port) );
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
    while((inb(0x64)&0x01)==0);         //阻塞
    return inb(0x60);
}


void turnoff()
{
    short* p;
    short port,val;
    p=(short*)0x4fc;
    port=*p;
    p=(short*)0x4fe;
    val=(*p)|0x2000;
    outw(port,val);
}


void init()
{
    u64* video=(u64*)0xa028;
    u64 base=*video;
    char* p=(char*)0xa019;
    char bpp=*p/8;

    int i;

    for(i=0;i<1024*768;i++)    //1024*768
    {
        video=(u64*)base;
        *video=0xffff00;
        base+=bpp;
    }
}


void point(int x,int y,int z)
{
    u64* video=(u64*)0xa028;
    u64 base=*video;
    char* p=(char*)0xa019;
    char bpp=*p/8;

    int* address;
    address=(int*)(base+(y*1024+x)*bpp);
    *address=z;
}


void draw(int x,int y,int z)
{
    point(x+512,384-y,z);
}


void circle()
{
    double angle=3.1415*2;
    int x,y; 
    while((angle-=0.001)>0)
    {
        x=(int)(250*cosine(angle));
        y=(int)(250*sine(angle));
        draw(x,y,0x000000);
    }
}


double absolute(double ret)
{
    return ret>=0?ret:-ret;
}


double squareroot(double x)
{
    double temp,ret;
    if(x<0){return 0;}

    temp=0.5*(1+x);
    ret=0.5*(temp+x/temp);
    while(absolute(ret-temp)>=1e-10)
    {
        temp=ret;
        ret=0.5*(temp+x/temp);
    }
return ret;
}


double ln(double a)
{
    int N = 15;//我们取了前15+1项来估算
    int k,nk;
    double x,xx,y;
    x = (a-1)/(a+1);
    xx = x*x;
    nk = 2*N+1;
    y = 1.0/nk;
    for(k=N;k>0;k--)
    {
        nk = nk - 2;
        y = 1.0/nk+xx*y;
    }
    return 2.0*x*y;
}


double logarithm(double x,double y)
{
    return ln(y)/ln(x);
}


double exponent(double x)
{
    double temp,ret = 1.0;
    int n = 1,i = 0;
    do{
        temp=1.0;
        for (i=1;i<=n;i++){temp = temp*x/i;}
        ret += temp;
        n++;
    }while(n<20);
return ret;
}


double power(double x,double y)
{
    return exponent(y*ln(x));
}


double cosine(double x)
{
    double ret=0,item=1.0,temp;
    int n=0,i,sign=1;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        temp=item;
        for(i=1;i<=2*n;i++)temp/=i;
        ret+=sign*temp;
        item*=x*x;
        sign *=-1;
        n++;
      }while (temp>1e-10);
return ret;
}


double sine(double x)
{
    int m = 1,i;
    double temp,ret = 0.0;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        i=0;
        if (m%2 == 0){temp= -1.0;}
        else{temp= 1.0;}
        for(i=1;i<=2*m-1;i++){temp = temp * x/i;}
        ret+= temp;
        m++;
    }while (temp<-.0000005||temp>0.0000005);
return ret;
}


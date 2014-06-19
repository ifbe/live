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


double squareroot(double x)
{
    double temp,ret;
    if(x<0){return 0;}

    temp=0.5*(1+x);
    ret=0.5*(temp+x/temp);
    while(1)
    {
	if( (ret-temp) >= 1e-10 ) break;
	if( (temp-ret) >= 1e-10 ) break;
        temp=ret;
        ret=0.5*(temp+x/temp);
    }
return ret;
}


void compute(double* ci,double* cj)
{
	double temp1=*ci;
	double temp2=*cj;
	*ci=temp1*temp1-temp2*temp2+0.03;
	*cj=2*temp1*temp2+0.7;
}


void main()
{
	init();

	double ci=0.03,cj=0.7;
	int x=0,y=0;
	while(x<1000)
	{
		printdouble(0,0,ci);
		printdouble(0,1,cj);

		compute(&ci,&cj);
		y=squareroot(ci*ci+cj*cj);
		if(y>0 && y<350)
		{
			point(x,y,0xffcc00);
		}
		else break;

		x++;
	}
}

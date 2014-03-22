#define s64 long long
#define u64 unsigned long long

void point(int x,int y,int color)
{
    s64* video=(s64*)0x3028;
    s64 base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=color;
}


void anscii(int x,int y,char ch)
{
    int i,j;
    s64 points=0xb000;
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

void string(int x,int y,char* p)
{
    while(*p!='\0')
    {
    anscii(x,y,*p);
    p++;
    x++;
    }
}

void hexadecimal(int x,int y,u64 z)
{
	char ch;
	int i=0;

	for(i=15;i>=0;i--)
	{
	ch=(char)(z&0x0000000f);
	z=z>>4;
	anscii(x+i,y,ch);
	}
}

void decimal(int x,int y,s64 z)
{
	char ch;
	int i=0;
	s64 temp;

	if(z<0){
	anscii(x,y,'-');
	x++;
	z=-z;
	}

	temp=z;
	while(temp>10){
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


void main()
{
	anscii(0,0,'$');
	string(0,1,"live is evil");
	decimal(0,2,-9223372036854775807);
	hexadecimal(0,3,0xfedcba9801234567);
}

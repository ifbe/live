#define u64 long long
typedef struct tree{
	int x1;
	int y1;
	struct tree* p1;
	int x2;
	int y2;
	struct tree* p2;
	int x3;
	int y3;
	struct tree* p3;
	int x4;
	int y4;
	struct tree* p4;
}tree;


static int table[10];
static int count=0;
static tree node[8];


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
        *video=0x44444444;
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


void anscii(int x,int y,char ch)
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
            else{point(j+x,i+y,0x44444444);}
        }
    rsi++;
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


void insert(int data)
{
}


void cubie(int x,int y,int z)
{
        int i,j;
        for(j=y*0x40;j<y*0x40+0x40;j++)
        {
		i=x*0x40;
                point(i,j,0);
                point(i+0x3f,j,0);
        }
        for(i=x*0x40;i<x*0x40+0x40;i++)
        {
		j=y*0x40;
                point(i,j,0);
                point(i,j+0x3f,0);
        }

        if(z!=0) decimal(x*8,y*8+2,z);
}


void btree()
{
    int i;
    init();
/*
    for(i=0;i<8;i++)
    {
	node[i].x1=node[i].y1=node[i].p1=0;
	node[i].x2=node[i].y2=node[i].p2=0;
	node[i].x3=node[i].y3=node[i].p3=0;
	node[i].x4=node[i].y4=node[i].p4=0;
    }
    for(i=0;i<8;i++) insert(table[i]);
    for(i=0;i<8;i++) cubie(i);
*/

	for(i=0;i<8;i++) cubie(i,0,table[i]);
}


void main()
{
    unsigned char buffer[128];
    int i;
    for(i=0;i<127;i++){buffer[i]=0;}
    for(i=0;i<8;i++) table[i]=0;

    init();
    count=0;
    string(55,20,"b+ tree!!!!");
    i=0;

    while(1)
    {
        buffer[i]=hltwait();
	if(buffer[i]>=0x80) continue;

        switch(buffer[i])
        {
            case 0x01: return;
            case 0x0e:{
                anscii(i-1,0,0x20);
                buffer[i]=0;
                if(i!=0)i--;
                break;
            }
            case 0x1c:{
		i=0;
    		table[count]=0;
    		while(buffer[i]!='\0')
    		{
		if( (buffer[i]<0x3a) && (buffer[i]>=0x30) )
		{
			table[count]=table[count]*10+(buffer[i]-0x30);
		}
		else break;

	        i++;
		}
    		count=(count+1)%8;
		for(i=0;i<128;i++) buffer[i]=0;

                btree();
		i=0;
                break;
            }
            default:{
                buffer[i]=convert(buffer[i]);  //进来一个convert一个
                string(0,0,buffer);
                i=(i+1)%127;
            }
        }
    }
}

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
	int temp=z&0xf0f0f0f0;
	if(temp==0) temp=0x773311+ z<<4;
	for(i=x*160;i<(x+1)*160;i++)
		for(j=y*160;j<(y+1)*160;j++)
			point(i,j,temp);

	decimal(8+x*20,4+y*10,z);

	for(i=x*160;i<(x+1)*160;i++)
	{
		point(i,y*160,0xffffffff);
		point(i,(y+1)*160,0xffffffff);
	}
	for(j=160*y;j<=(y+1)*160;j++)
	{
		point(160*x,j,0xffffffff);
		point(160*(x+1),j,0xffffffff);
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
	int table[4][4];
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
				temp=j*4 +i;
				table[i][j]=power(3);
		}
	}

	char key=0;
	while(1)
	{
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				cubie(i,j,table[j][i]);

		key=keyboard();

		if(key==0x1){break;}
		if(key==0x4b)	//left
		{
		for(i=0;i<4;i++)
		{
			int current=0;
			int k;
			int temp;
			for(j=0;j<3;j++)
			{
				if(table[i][j] != 0)
				{
				for(k=j+1;k<4;k++)
				{
					if(table[i][k]!=0)
					{

					if(table[i][k]!=table[i][j])
					{
						break;
					}

					temp=2*table[i][j];
					table[i][j]=0;
					table[i][k]=0;
					table[i][current]=temp;
					current++;
					break;

					}
				}
				}
			}
			if(table[i][3]!=0)
			{
				temp=table[i][3];
				table[i][3]=0;
				table[i][current]=temp;
			}
		}
		}

		if(key==0x4d)	//right
		{
		for(i=0;i<4;i++)
		{
			int current=3;
			int k;
			int temp;
			for(j=3;j>0;j--)
			{
				if(table[i][j] != 0)
				{
				for(k=j-1;k>=0;k--)
				{
					if(table[i][k]!=0)
					{

					if(table[i][k]!=table[i][j]) break;

					temp=2*table[i][j];
					table[i][j]=0;
					table[i][k]=0;
					table[i][current]=temp;
					current--;
					break;

					}
				}
				}
			}
			if(table[i][0]!=0)
			{
				temp=table[i][0];
				table[i][0]=0;
				table[i][current]=temp;
			}
		}
		}
		if(key==0x48)	//up
		{
		for(j=0;j<4;j++)
		{
			int current=0;
			int k;
			int temp;
			for(i=0;i<3;i++)
			{
				if(table[i][j] != 0)
				{
				for(k=i+1;k<4;k++)
				{
					if(table[k][j]!=0)
					{

					if(table[k][j]!=table[i][j]) break;

					temp=2*table[i][j];
					table[i][j]=0;
					table[k][j]=0;
					table[current][j]=temp;
					current++;
					break;

					}
				}
				}
			}
			if(table[3][j]!=0)
			{
				temp=table[3][j];
				table[3][j]=0;
				table[current][j]=temp;
			}
		}
		}
		if(key==0x50)	//down
		{
		for(j=0;j<4;j++)
		{
			int current=3;
			int k;
			int temp;
			for(i=3;i>0;i--)
			{
				if(table[i][j] != 0)
				{
				for(k=i-1;k>=0;k--)
				{
					if(table[k][j]!=0)
					{

					if(table[k][j]!=table[i][j]) break;

					temp=2*table[i][j];
					table[i][j]=0;
					table[k][j]=0;
					table[current][j]=temp;
					current--;
					break;

					}
				}
				}
			}
			if(table[0][j]!=0)
			{
				temp=table[0][j];
				table[0][j]=0;
				table[current][j]=temp;
			}
		}
		}
	}
}

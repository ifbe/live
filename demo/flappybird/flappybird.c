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
    u64 points=0x6000;
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


int random()
{
        int key,i=0;
        char* memory=(char*)0x0;
        for(i=0;i<0x1000;i++)
                key+=memory[i];
	if(key<0) key=-key;
        return key;
}


void main()
{
	int i,j;
	int x1,y1,x2,y2,x3,y3,x4,y4;
	for(i=0;i<1024;i++)
		for(j=0;j<768;j++)
			point(i,j,0);

	x1=100;y1=400;
	x2=400;y2=200;
	x3=700;y3=300;
	x4=1000;y4=500;

	int20();

	while(1)
	{
		//print world
		i=x1;
		if(i<=0)	i=0;
		for(;i<x1+100;i++)
		{
			for(j=0;j<y1-100;j++)
				point(i,j,0xff00);
			for(j=y1+100;j<768;j++)
				point(i,j,0xff00);
		}
		for(i=x2;i<x2+100;i++)
		{
			for(j=0;j<y2-100;j++)
				point(i,j,0xff00);
			for(j=y2+100;j<768;j++)
				point(i,j,0xff00);
		}
		for(i=x3;i<x3+100;i++)
		{
			for(j=0;j<y3-100;j++)
				point(i,j,0xff00);
			for(j=y3+100;j<768;j++)
				point(i,j,0xff00);
		}
		int temp=x4+100;
		if(temp>1024)	temp=1024;
		for(i=x4;i<temp;i++)
		{
			for(j=0;j<y4-100;j++)
				point(i,j,0xff00);
			for(j=y4+100;j<768;j++)
				point(i,j,0xff00);
		}

		//print bird

		//wait
		char key=hltwait();
		if(key==0x01) break;
		else if(key==0x39)
		{
		}
		else if(key== -1)
		{
			for(i=0;i<1024;i++){
				for(j=0;j<768;j++)
				{
					point(i,j,0);
				}
			}
	
			x1-=16;
			x2-=16;
			x3-=16;
			x4-=16;

			if(x1< -100){
				x1=x2;
				x2=x3;
				x3=x4;
				x4+=300;
				y1=y2;
				y2=y3;
				y3=y4;
				y4=random() %600;
			}
		}

		//one line full ?

		//next loop
	}
}

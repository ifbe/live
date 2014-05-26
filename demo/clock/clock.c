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
	x=128*x;
	y=256*y;

	for(i=0;i<16;i++)
	{
	p=(char*)points;

	for(j=0;j<8;j++)
	{
		temp=*p;
		temp=temp<<j;
		temp&=0x80;
		int tempx,tempy;

		if(temp!=0)
		{
			for(tempx=16*j;tempx<16*j+16;tempx++)
				for(tempy=16*i;tempy<16*i+16;tempy++)
					point(x+tempx,y+tempy,0xffffffff);
		}
		else
		{
			for(tempx=16*j;tempx<16*j+16;tempx++)
				for(tempy=16*i;tempy<16*i+16;tempy++)
					point(x+tempx,y+tempy,0x44444444);
		}
	}
	points++;
}
}



clock()
{
	unsigned char century=port(0x32);			//century
	unsigned char year=port(9);				//year
	unsigned char month=port(8);				//month
	unsigned char day=port(7);				//day
	unsigned char hour=port(4);				//hour
	unsigned char minute=port(2);				//minute
	unsigned char second=port(0);				//second




	if(hour<2) hour+=8;			//0->8,1->9
	else if(hour<0x10) hour=hour+8+6;	//2->10,3->11,4->12......
	else if(hour<0x12) hour+=8;		//10->18,11->19
	else if(hour<0x16) hour=hour+8+6;	//12->20,13->21
	else{					//16->0,17->1.......23->7
		hour-=0x16;			//进位
		//......

	}

	anscii(0,0,' ');
	anscii(1,0,' ');
	anscii(2,0,(century & 0xf0) >> 4);
	anscii(3,0,century & 0xf);
	anscii(4,0,(year & 0xf0) >> 4);
	anscii(5,0,year & 0xf);
	anscii(6,0,' ');
	anscii(7,0,' ');

	anscii(0,1,' ');
	anscii(1,1,(month & 0xf0) >> 4);
	anscii(2,1,month & 0xf);
	anscii(3,1,',');
	anscii(4,1,(day & 0xf0) >> 4);
	anscii(5,1,day & 0xf);
	anscii(6,1,'.');
	anscii(7,1,' ');

	anscii(0,2,(hour & 0xf0) >> 4);
	anscii(1,2,hour & 0xf);
	anscii(2,2,':');
	anscii(3,2,(minute & 0xf0) >> 4);
	anscii(4,2,minute & 0xf);
	anscii(5,2,':');
	anscii(6,2,(second & 0xf0) >> 4);
	anscii(7,2,second & 0xf);
}

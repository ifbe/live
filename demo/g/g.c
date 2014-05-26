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


int random()
{
        int key,i=0;
        char* memory=(char*)0x0;
        for(i=0;i<0x1000;i++)
                key+=memory[i];
        return key;
}


void main()
{
	int x,y;
	int x0,y0;
	int speed,t;
	int score,release;
	int targetx,targety;
	int i,j;

	for(i=0;i<1024;i++)
		for(j=0;j<768;j++)
			point(i,j,0);

	string(0,0,"speed:");
	string(0,2,"score:");

	x=x0=100;
	y=y0=0;
	speed=0;
	t=0;
	score=0;
	targetx=random() %668+100;
	targety=random() %512;

	int20();

	while(1)
	{
		//print world
		for(j=0;j<768;j++) point(100,j,0xffffffff);
		for(i=x;i<x+16;i++)
			for(j=y;j<y+16;j++)
				point(i,j,0xffffffff);
		for(i=targetx;i<targetx+64;i++)
			for(j=targety;j<targety+64;j++)
				point(i,j,0xffffffff);

		for(i=0;i<100;i++)
			for(j=16;j<32;j++)
				point(i,j,0);
		decimal(0,1,speed);
		decimal(0,3,score);

		//wait
		unsigned char key=hltwait();
		for(i=x;i<x+16;i++)
			for(j=y;j<y+16;j++)
				point(i,j,0);

		if(key==0x01) break;
		else if(key==0x39){
			if(release==0) speed++;
		}
		else if(key==0xb9){
			release=1;
		}
		else if(key==0x48){
			if(y0>=16)
			{
				y0-=16;
				y-=16;
			}
		}
		else if(key==0x50){
			if(y0<752)
			{
				y0+=16;
				y+=16;
			}
		}
		else if(key== 0xff){
		if(release==1){
			t++;
			x=x0+speed*t;
			y=y0+5*t*t;		//0.5*10*t^2

			if(x>1024 | y>768)
			{
				//hit ?
				double temp1;
				double temp2;
temp1=(double)( 5*(targetx-100)*(targetx-100) ) / ( (double)(targety+64-y0) );
temp2=(double)( 5*(targetx-36)*(targetx-36) ) / ( (double)(targety-y0) );
			if((int)temp1<speed*speed && (int)temp2>speed*speed)
			{
				for(i=targetx;i<targetx+64;i++)
					for(j=targety;j<targety+64;j++)
						point(i,j,0);
				score++;
				targetx=random() %668+100;
				targety=random() %512;
			}
				release=0;
				x0=100;
				y0=0;
				x=100;
				y=0;
				t=0;
				speed=0;
			}
		}
		}

		//one line full ?

		//next loop
	}

	shutup20();
}

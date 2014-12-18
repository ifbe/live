static int x,y;
static int x0,y0;
static int speed,t;
static int score,release;
static int targetx,targety;
static int i,j;

void init()
{
	for(i=0;i<1024;i++)
		for(j=0;j<768;j++)
			point(i,j,0x44444444);

	string(0,0,"speed:");
	string(0,2,"score:");

	x=x0=100;
	y=y0=0;
	speed=0;
	t=0;
	score=0;
	targetx=random() %668+100;
	targety=random() %512;
}
void printworld()
{
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

	writescreen();
}
void cleanup()
{
	for(i=x;i<x+16;i++)
		for(j=y;j<y+16;j++)
			point(i,j,0x44444444);
}
void ticktock()
{
	cleanup();
	if(release==1)
	{
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
void main()
{
	init();

	while(1)
	{
		//print world
		printworld();

		//wait
		int key=waitevent();

		//change
		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 0x20:		//press
			{
				if(release==0) speed++;
				break;
			}
			case 0xa0:		//release
			{
				release=1;
				break;
			}
			case 0x40000052:	//up
			{
				cleanup();
				if(y0>=16)
				{
					y0-=16;
					y-=16;
				}
				break;
			}
			case 0x40000051:	//down
			{
				cleanup();
				if(y0<752)
				{
					y0+=16;
					y+=16;
				}
				break;
			}
			case 0xff:
			{
				ticktock();
				break;
			}
			default:continue;
		}
	
	}

}

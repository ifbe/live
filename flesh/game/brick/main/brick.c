void cubie(x,y,color)
{
	int i,j;
	if(color!=0) color=0xffffffff;
	for(i=x;i<x+16;i++)
		for(j=y;j<y+16;j++)
			point(i,j,color);

	for(i=x;i<x+16;i++){
		point(i,y,0);
		point(i,y+15,0);
	}
	for(j=y;j<y+16;j++){
		point(x,j,0);
		point(x+15,j,0);
	}
}

typedef struct somestruct{
	int x;
	int y;
	int dx;
	int dy;
}somestruct;
static unsigned long long table[24];
static somestruct ball;
static int position=512;

void init()
{
	int i,j;

	for(i=0;i<24;i++) table[i]=0xffffffffffffffff;
	ball.x=512;
	ball.y=684;
	ball.dx=random() & 0xf;
	ball.dy=16;

	//init screen
	for(i=0;i<1024;i++)		//clear screen
		for(j=0;j<768;j++)
			point(i,j,0);
}
void printworld()
{
	int i,j;
	unsigned long long temp;
	//print brick
	for(j=0;j<24;j++)
	{
		temp=table[j];
		for(i=0;i<64;i++)
		{
			cubie(i*16,j*16,temp & 0x1);
			temp/=2;    //temp>>1 wrong,do not know why
		}
	}
	//print board
	for(i=position-64;i<position+64;i++)
	{
		point(i,700,0xffffffff);
	}
	//print ball
	cubie(ball.x,ball.y,0xffffffff);
	//
	writescreen();
}
void cleanup()
{
	int i;
	for(i=position-64;i<position+64;i++)
	{
		point(i,700,0);
	}
	//clear ball
	cubie(ball.x,ball.y,0);
	//move board
}
void left()
{
	cleanup();
	if(position>80)
	{
		position-=0x16;
	}
}
void right()
{
	cleanup();
	if(position<1024-80)
	{
		position+=0x16;
	}
}
void time()
{
	cleanup();
	
	//ball moving
	if(ball.x<16) ball.dx= -ball.dx;
	if(ball.x>992) ball.dx= -ball.dx;
	if(ball.y<16) ball.dy= -ball.dy;
	if(ball.y>668)
	{
		if(ball.x>position-64 && ball.x<position+64)
		{
			ball.dy= -ball.dy;
		}
	}
	ball.x+=ball.dx;
	ball.y+=ball.dy;
	
	//吃子儿
	if(ball.y<24*16)
	{
		int line=ball.y / 16;
		int bit=ball.x / 16;
		unsigned long long temp=(unsigned long long)0x1<<bit;
		temp|=(unsigned long long)0x1<<(bit+1);
		temp=~temp;
		table[line]&=temp;
	}
}


void main()
{
	//第0步:初始化
	init();

	while(1)
	{
		//第1步:显示世界
		printworld();

		//第2步:等待输入
		int key=waitevent();

		//clear board
		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 0x40000050:
			{
				left();
				break;
			}
			case 0x4000004f:
			{
				right();
				break;
			}
			case 0xff:
			{
				time();
				break;
			}
			default:continue;
		}
		if(ball.y>750)return;
	}

}
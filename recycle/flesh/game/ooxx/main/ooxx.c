double cosine(double x);
double sine(double x);
static int x1=0,y1=0,x2=0,y2=0;
static int table[3][3];


void cubie(int x,int y,int z)
{
	int i,j;

	//背景色
	for(i=256*x;i<256*x+256;i++)
		for(j=256*y;j<256*y+256;j++)
			point(i,j,0x44444444);

	for(i=256*x;i<256*x+256;i++)
	{
		point(i,256*y,0);
		point(i,256*y+255,0);
	}
	for(j=256*y;j<256*y+256;j++)
	{
		point(256*x,j,0);
		point(256*x+255,j,0);
	}

	if(z==1)	//画0
	{
		double a;
		for(a=0;a<6.28;a+=0.01)
		{
		point(		(int)(256*x+128+100*cosine(a)),
				(int)(256*y+128+100*sine(a)),
				0xff0000
		);
		point(
				(int)(256*x+129+100*cosine(a)),
				(int)(256*y+128+100*sine(a)),
				0xff0000
		);
		}
	}
	if(z==2)	//画x
	{
		for(i=28;i<228;i++)
		{
			point(256*x+i,256*y+i,0xff);
			point(256*x+i+1,256*y+i,0xff);
			point(256*x+i,256*y+256-i,0xff);
			point(256*x+i+1,256*y+256-i,0xff);
		}
	}
}


void kuang(x,y,color)
{
	int i,j;
	for(i=x*256;i<x*256+256;i++)
	{
		point(i,y*256,color);
		point(i,y*256+1,color);
		point(i,y*256+254,color);
		point(i,y*256+255,color);
	}
	for(j=y*256;j<y*256+256;j++)
	{
		point(x*256,j,color);
		point(x*256+1,j,color);
		point(x*256+254,j,color);
		point(x*256+255,j,color);
	}
}




int check()
{
	if(table[0][0]==table[0][1]&&table[0][1]==table[0][2])
	{
		if(table[0][0]==1)	return 1;
		if(table[0][0]==2)	return 2;
	}
	if(table[1][0]==table[1][1]&&table[1][1]==table[1][2])
	{
		if(table[1][0]==1)	return 1;
		if(table[1][0]==2)	return 2;
	}
	if(table[2][0]==table[2][1]&&table[2][1]==table[2][2])
	{
		if(table[2][0]==1)	return 1;
		if(table[2][0]==2)	return 2;
	}
	if(table[0][0]==table[1][0]&&table[1][0]==table[2][0])
	{
		if(table[0][0]==1)	return 1;
		if(table[0][0]==2)	return 2;
	}
	if(table[0][1]==table[1][1]&&table[1][1]==table[2][1])
	{
		if(table[0][1]==1)	return 1;
		if(table[0][1]==2)	return 2;
	}
	if(table[0][2]==table[1][2]&&table[1][2]==table[2][2])
	{
		if(table[0][2]==1)	return 1;
		if(table[0][2]==2)	return 2;
	}
	if(table[0][0]==table[1][1]&&table[1][1]==table[2][2])
	{
		if(table[1][1]==1)	return 1;
		if(table[1][1]==2)	return 2;
	}
	if(table[0][2]==table[1][1]&&table[1][1]==table[2][0])
	{
		if(table[1][1]==1)	return 1;
		if(table[1][1]==2)	return 2;
	}

	if(	table[0][0]!=0&&table[0][1]!=0&&table[0][2]!=0
	&&	table[1][0]!=0&&table[1][1]!=0&&table[1][2]!=0
	&&	table[2][0]!=0&&table[2][1]!=0&&table[2][2]!=0)
	{return 3;}

	return 0;
}


void init()
{
	int i,j;

	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			table[i][j]=0;

	for(i=0;i<1024;i++)
		for(j=0;j<768;j++)
			point(i,j,0x44444444);
}
void printworld()
{
	int i,j;
	
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			cubie(i,j,table[j][i]);
	kuang(x1,y1,0xff0000);
	kuang(x2,y2,0xff);
	
	writescreen();
}
void main()
{
	init();

	while(1)
	{
		//printworld
		printworld();

		//wait
		int key=waitevent();

		//分发
		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 'a':
			{
				if(x1>0) x1--;
				break;
			}
			case 'd':
			{
				if(x1<2) x1++;
				break;
			}
			case 'w':
			{
				if(y1>0) y1--;
				break;
			}
			case 's':
			{
				if(y1<2) y1++;
				break;
			}
			case 0x20:
			{
				if(table[y1][x1] == 0)
				{
					table[y1][x1]=1;
				}
				int temp=check();
				if(temp==1){string(100,0,"red win !!!!");}
				if(temp==3){string(100,0,"no winner");}
				
				break;
			}
			case 0x40000050:		//left
			{
				if(x2>0) x2--;
				break;
			}
			case 0x4000004f:		//right
			{
				if(x2<2) x2++;
				break;
			}
			case 0x40000052:		//up
			{
				if(y2>0) y2--;
				break;
			}
			case 0x40000051:		//down
			{
				if(y2<2) y2++;
				break;
			}
			case 0xd:
			{
				if(table[y2][x2] == 0)
				{
					table[y2][x2]=2;
				}
				int temp=check();
				if(temp==2){string(100,0,"blue win !!!!");}
				if(temp==3){string(100,0,"no winner");}
				
				break;
			}
			default:continue;
		}
	}
}

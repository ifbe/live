#define u64 long long
static int table[16][16];
static int x=0,y=0;


void cubie(x,y,z)
{
	int i,j;
	x=40*x;
	y=40*y;
	for(i=0;i<40;i++)
		for(j=0;j<40;j++)
			point(x+i,y+j,z);
}


void printworld()
{
	int i,j;

	for(i=0;i<1024;i++)
	{
		for(j=0;j<768;j++)
		{
			point(i,j,0x44444444);
		}
	}
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			cubie(i,j,table[i][j]);
			point(i+720,j+320,table[i][j]);
		}
	}

	for(j=40;j<=640;j+=40)			//heng
		for(i=0;i<640;i++)
			point(i,j,0xffffffff);
	for(i=40;i<=640;i+=40)			//shu
		for(j=0;j<=640;j++)
			point(i,j,0xffffffff);

	for(i=40*y;i<40*(y+1);i++)		//kuang heng
	{
		point(40*x,i,0xff0000);
		point(40*(x+1),i,0xff0000);
	}
	for(i=40*x;i<40*(x+1);i++)		//kuang shu
	{
		point(i,40*y,0xff0000);
		point(i,40*(y+1),0xff0000);
	}
	for(i=712;i<744;i++)			//xiao kuang heng
	{
		point(i,312,0xff0000);
		point(i,344,0xff0000);
	}

	for(i=312;i<344;i++)			//xiao kuang shu
	{
		point(712,i,0xff0000);
		point(744,i,0xff0000);
	}

	int theword=0;
	for(j=0;j<16;j++)
	{
		theword=0;
		for(i=0;i<16;i++)
		{
			theword*=2;
			if(table[i][j]<0){theword++;}
		}
		hexadecimal(80,2*j,theword);
	}
	
	writescreen();
}
void main()
{
	int i,j;
	for(i=0;i<16;i++)
		for(j=0;j<16;j++)
			table[i][j]=0;

	while(1)
	{
		printworld();
		
		int key=waitevent();

		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 0x40000050:		//left
			{
				if(x>0)	x--;
				break;
			}
			case 0x4000004f:		//right
			{
				if(x<15)	x++;
				break;
			}
			case 0x40000052:		//up
			{
				if(y>0)	y--;
				break;
			}
			case 0x40000051:		//down
			{
				if(y<15)	y++;
				break;
			}
			case 0x20:	//space
			{
				table[x][y]=~table[x][y];
				break;
			}
		}
	}
}

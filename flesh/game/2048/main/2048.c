void cubie(int x,int y,int z)
{
	int i,j,color;
	for(j=y*160;j<(y+1)*160;j++)
	{
		for(i=x*160;i<x*160+5;i++)
		{
			point(i,j,0x44444444);
			point(i+155,j,0x44444444);
		}
	}
	for(i=x*160;i<(x+1)*160;i++)
	{
		for(j=y*160;j<y*160+5;j++)
		{
			point(i,j,0x44444444);
			point(i,j+155,0x44444444);
		}
	}

	switch(z)
	{
		case 0:color=0x55555555;break;
		case 2:color=0xfffffff0;break;
		case 4:color=0xffffffc0;break;
		case 8:color=0x995000;break;
		case 16:color=0xc05000;break;
		case 32:color=0xb03000;break;
		case 64:color=0xff0000;break;
		case 128:color=0xffffa0;break;
		case 256:color=0xffff80;break;
		case 512:color=0xffff00;break;
		case 1024:color=0xffffb0;break;
		case 2048:color=0xffffffff;break;
		case 4096:color=0xffffffff;break;
	}
	for(i=x*160+5;i<x*160+155;i++)
		for(j=y*160+5;j<y*160+155;j++)
			point(i,j,color);

	if(z!=0) decimal(4+x*10,4+y*10,z);
}
int power(int in)
{
	if(in==0) return 1;

	int temp=2;
	for(in-=1;in>0;in--) temp*=2;
	return temp;
}




static int table[4][4];


void init()
{
	int i,j,temp;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
				table[i][j]=0;
		}
	}
	table[random() & 0x3][random() & 0x3]=power( (random() &0x1) +1 );
}
void printworld()
{
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			cubie(i,j,table[j][i]);
	writescreen();
}
void newone()
{
	int i,j;
	int count=0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(table[i][j]==0) count++;
		}
	}
	if(count!=0)
	{
		int temp=random() %count;					//新出来的放在哪儿
		for(i=0;i<4;i++)
		{
		for(j=0;j<4;j++)
		{
			if( (table[i][j] == 0) && (temp==0) )	//里面没东西并且就选这一个
			{
				table[i][j]=power((random() & 0x1) +1 );
				return;
			}
			else temp--;
		}
		}
	}
}
void left()
{
	int i,j;
	for(i=0;i<4;i++)
	{
		int count=0;
		int temp;
		for(j=0;j<4;j++)	//整理
		{
			if(table[i][j] != 0)
			{
				temp=table[i][j];
				table[i][j]=0;
				table[i][count++]=temp;
			}
		}

		//if(count==1)
		if(count==2)
		{
			if(table[i][0]==table[i][1])
			{
				table[i][0] *=2;
				table[i][1]=0;
			}
		}
		if(count==3)
		{
			if(table[i][0]==table[i][1])
			{
				table[i][0] *=2;
				table[i][1]=table[i][2];
				table[i][2]=0;
			}
			else if(table[i][1]==table[i][2])
			{
				table[i][1] *=2;
				table[i][2]=0;
			}
		}
		if(count==4)
		{
			if(table[i][0]==table[i][1])
			{
				table[i][0] *=2;
				table[i][1]=table[i][2];
				table[i][2]=table[i][3];
				table[i][3]=0;
					if(table[i][1]==table[i][2]){
					table[i][1] *=2;
					table[i][2]=0;
				}
			}
			else if(table[i][1]==table[i][2])
			{
				table[i][1] *=2;
				table[i][2]=table[i][3];
				table[i][3]=0;
			}
			else if(table[i][2]==table[i][3])
			{
				table[i][2] *=2;
				table[i][3]=0;
			}
		}
	}
	
	newone();
}
void right()
{
	int i,j;
	for(i=0;i<4;i++)
	{
		int count=0;
		int temp;
		for(j=3;j>=0;j--)
		{
		if(table[i][j] != 0)
		{
			temp=table[i][j];
			table[i][j]=0;
			table[i][3-count]=temp;
			count++;
		}
		}

		//if(count==1)
		if(count==2){
			if(table[i][3]==table[i][2]){
				table[i][3] *=2;
				table[i][2]=0;
			}
		}
		if(count==3)
		{
			if(table[i][3]==table[i][2])
			{
				table[i][3] *=2;
				table[i][2]=table[i][1];
				table[i][1]=0;
			}
			else if(table[i][2]==table[i][1])
			{
				table[i][2] *=2;
				table[i][1]=0;
			}
		}
		if(count==4)
		{
			if(table[i][3]==table[i][2])
			{
				table[i][3] *=2;
				table[i][2]=table[i][1];
				table[i][1]=table[i][0];
				table[i][0]=0;
				if(table[i][1]==table[i][2])
				{
					table[i][2] *=2;
					table[i][1]=0;
				}
			}
			else if(table[i][2]==table[i][1])
			{
				table[i][2] *=2;
				table[i][1]=table[i][0];
				table[i][0]=0;
			}
			else if(table[i][1]==table[i][0])
			{
				table[i][1] *=2;
				table[i][0]=0;
			}
		}
	}

	newone();
}
void up()
{
	int i,j;
	for(j=0;j<4;j++)
	{
		int count=0;
		int temp;
		for(i=0;i<4;i++)
		{
			if(table[i][j] != 0)
			{
				temp=table[i][j];
				table[i][j]=0;
				table[count++][j]=temp;
			}
		}
		//if(count==1)
		if(count==2)
		{
			if(table[0][j]==table[1][j])
			{
				table[0][j] *=2;
				table[1][j]=0;
			}
		}
		if(count==3)
		{
			if(table[0][j]==table[1][j])
			{
				table[0][j] *=2;
				table[1][j]=table[2][j];
				table[2][j]=0;
			}
			else if(table[1][j]==table[2][j])
			{
				table[1][j] *=2;
				table[2][j]=0;
			}
		}
		if(count==4)
		{
			if(table[0][j]==table[1][j])
			{
				table[0][j] *=2;
				table[1][j]=table[2][j];
				table[2][j]=table[3][j];
				table[3][j]=0;
				if(table[1][j]==table[2][j])
				{
					table[1][j] *=2;
					table[2][j]=0;
				}
			}
			else if(table[1][j]==table[2][j])
			{
				table[1][j] *=2;
				table[2][j]=table[3][j];
				table[3][j]=0;
			}
			else if(table[2][j]==table[3][j])
			{
				table[2][j] *=2;
				table[3][j]=0;
			}
		}
	}

	newone();
}
void down()
{
	int i,j;
	for(j=0;j<4;j++)
	{
		int count=0;
		int temp;
		for(i=3;i>=0;i--)
		{
			if(table[i][j] != 0)
			{
				temp=table[i][j];
				table[i][j]=0;
				table[3-count][j]=temp;
				count++;
			}
		}
		
		//if(count==1)
		if(count==2)
		{
			if(table[3][j]==table[2][j])
			{
				table[3][j] *=2;
				table[2][j]=0;
			}
		}
		if(count==3)
		{
			if(table[3][j]==table[2][j])
			{
				table[3][j] *=2;
				table[2][j]=table[1][j];
				table[1][j]=0;
			}
			else if(table[1][j]==table[2][j]){
				table[2][j] *=2;
				table[1][j]=0;
			}
		}
		if(count==4)
		{
			if(table[3][j]==table[2][j])
			{
				table[3][j] *=2;
				table[2][j]=table[1][j];
				table[1][j]=table[0][j];
				table[0][j]=0;
				if(table[1][j]==table[2][j])
				{
					table[2][j] *=2;
					table[1][j]=0;
				}
			}
			else if(table[2][j]==table[1][j])
			{
				table[2][j] *=2;
				table[1][j]=table[0][j];
				table[0][j]=0;
			}
			else if(table[1][j]==table[0][j])
			{
				table[1][j] *=2;
				table[0][j]=0;
			}
		}
	}

	newone();
}


void main()
{
	//第0步:初始化
	init();

	while(1)
	{
		//第1步:显示世界
		printworld();

		//第2步:等待虫子
		int key=waitevent();

		//第3步:各种虫子咬自己的饼以及世界改变
		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 0x40000050:					//left
			{
				left();
				break;
			}
			case 0x4000004f:					//right
			{
				right();
				break;
			}
			case 0x40000052:					//up
			{
				up();
				break;
			}
			case 0x40000051:					//down
			{
				down();
				break;
			}
			default:continue;
		}
	}
}
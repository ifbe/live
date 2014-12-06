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




void cubie(int x,int y,int z)
{
        int i,j;
		
		//竖线
        for(j=y*0x40;j<y*0x40+0x40;j++)
        {
			i=x*0x40;
			point(i,j,0);
			point(i+0x3f,j,0);
        }

		//横线
        for(i=x*0x40;i<x*0x40+0x40;i++)
        {
			j=y*0x40;
			point(i,j,0);
			point(i,j+0x3f,0);
        }

		//框里的数字
        if(z!=0) decimal(x*8,y*8+2,z);
}


void btree()
{
	int i,j;
	
	//清空方块
	for(i=0;i<0x40*8;i++)
		for(j=0;j<0x40;j++)
			point(i,j,0x44444444);

	for(i=0;i<8;i++) cubie(i,0,table[i]);
}


void insert(int data)
{
}


void main()
{
	//第0步:初始化
	int x,y,i;
	unsigned char buffer[128];

	for(x=0;x<1024;x++)    //1024*768
	{
		for(y=0;y<768;y++)
		{
			point(x,y,0x44444444);
		}
	}

	for(i=0;i<127;i++)
	{
		buffer[i]=0;
	}

	for(i=0;i<8;i++)
	{
		table[i]=0;
	}

	count=0;
	i=0;
	string(55,20,"b+ tree!!!!");

	while(1)
	{
		//第1步:显示世界
		string(0,0,buffer);
		writescreen();


		//第2步:等待虫子
		int temp=waitevent();
		if(temp<=0) return;


		//第3步:放虫子改变世界
		buffer[i]=temp;
		switch(buffer[i])
		{
			case 0x1b: return;			//esc
			case 0x08:					//删除
			{
				anscii(i-1,0,0x20);
				buffer[i]=0;
				if(i!=0)i--;

				break;
			}
			case 0xd:					//回车
			{
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
			default:
			{
				string(0,0,buffer);
				i=(i+1)%127;
			}
		}//switch

	}//while
}
typedef struct stucture
{
	int x;
	int y;
	int type;
	int direction;
	int x1;
	int y1;
	int x2;
	int y2;
	int x3;
	int y3;
	int x4;
	int y4;
}structure;
static int score=0;
static unsigned int table[49];
static structure that;


void cubie(int x,int y,int z)
{
	int i,j,color;
	x+=16;
	color=z>0?0xffffffff:0;

	for(i=x*16;i<(x+1)*16;i++){
		for(j=y*16;j<(y+1)*16;j++){
			point(i,j,color);
		}
	}

	for(i=x*16;i<x*16+16;i++){
		point(i,y*16,0x44444444);
		point(i,y*16+16,0x44444444);
	}
	for(j=y*16;j<y*16+16;j++){
		point(x*16,j,0x44444444);
		point(x*16+16,j,0x44444444);
	}
}




void generate(structure* that)
{
	if(that->type==0)
	{
		if(that->direction==1 | that->direction==3)
		{						//	0000
			that->x1=that->x;
			that->x2=that->x +1;
			that->x3=that->x +2;
			that->x4=that->x +3;
			that->y1=that->y2=that->y3=that->y4=that->y;
		}
		if(that->direction==0 | that->direction==2)
		{
			that->x1=that->x2=that->x3=that->x4=that->x;
			that->y1=that->y;
			that->y2=that->y +1;
			that->y3=that->y +2;
			that->y4=that->y +3;
		}
	}
	if(that->type==1)
	{
		if(that->direction==0)				//	000
		{						//	 0
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x -1;
			that->y2=that->y;
			that->x3=that->x +1;
			that->y3=that->y;
			that->x4=that->x;
			that->y4=that->y +1;
		}						//	0
		if(that->direction==1)				//	00
		{						//	0
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x;
			that->y2=that->y -1;
			that->x3=that->x;
			that->y3=that->y +1;
			that->x4=that->x +1;
			that->y4=that->y;
		}
		if(that->direction==2)				//	 0
		{						//	000
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x -1;
			that->y2=that->y;
			that->x3=that->x +1;
			that->y3=that->y;
			that->x4=that->x;
			that->y4=that->y -1;
		}						//	 0
		if(that->direction==3)				//	00
		{						//	 0
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x -1;
			that->y2=that->y;
			that->x3=that->x;
			that->y3=that->y +1;
			that->x4=that->x;
			that->y4=that->y -1;
		}
	}
	if(that->type==2)
	{							//	0
		if(that->direction==0|that->direction==2)	//	00
		{						//	 0
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x -1;
			that->y2=that->y;
			that->x3=that->x -1;
			that->y3=that->y -1;
			that->x4=that->x;
			that->y4=that->y+1;
		}
		if(that->direction==1|that->direction==3)	//	 00
		{						//	00
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x +1;
			that->y2=that->y;
			that->x3=that->x;
			that->y3=that->y +1;
			that->x4=that->x -1;
			that->y4=that->y +1;
		}
	}
	if(that->type==3)
	{							//	 0
		if(that->direction==0|that->direction==2)	//	00
		{						//	0
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x +1;
			that->y2=that->y;
			that->x3=that->x;
			that->y3=that->y +1;
			that->x4=that->x +1;
			that->y4=that->y -1;
		}
		if(that->direction==1|that->direction==3)	//	00
		{						//	 00
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x +1;
			that->y2=that->y;
			that->x3=that->x;
			that->y3=that->y -1;
			that->x4=that->x -1;
			that->y4=that->y -1;
		}
	}
	if(that->type==4)
	{							//	00
		if(that->direction==0)				//	0
		{						//	0
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x;
			that->y2=that->y -1;
			that->x3=that->x;
			that->y3=that->y +1;
			that->x4=that->x +1;
			that->y4=that->y -1;
		}
		if(that->direction==1)				//	0
		{						//	000
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x -1;
			that->y2=that->y;
			that->x3=that->x +1;
			that->y3=that->y;
			that->x4=that->x -1;
			that->y4=that->y -1;
		}						//	 0
		if(that->direction==2)				//	 0
		{						//	00
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x;
			that->y2=that->y -1;
			that->x3=that->x;
			that->y3=that->y +1;
			that->x4=that->x -1;
			that->y4=that->y +1;
		}
		if(that->direction==3)				//	000
		{						//	  0
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x -1;
			that->y2=that->y;
			that->x3=that->x +1;
			that->y3=that->y;
			that->x4=that->x +1;
			that->y4=that->y +1;
		}
	}
	if(that->type==5)
	{							//	00
		if(that->direction==0)				//	 0
		{						//	 0
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x;
			that->y2=that->y -1;
			that->x3=that->x;
			that->y3=that->y +1;
			that->x4=that->x -1;
			that->y4=that->y -1;
		}
		if(that->direction==1)				//	000
		{						//	0
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x -1;
			that->y2=that->y;
			that->x3=that->x +1;
			that->y3=that->y;
			that->x4=that->x -1;
			that->y4=that->y +1;
		}						//	0
		if(that->direction==2)				//	0
		{						//	00
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x;
			that->y2=that->y -1;
			that->x3=that->x;
			that->y3=that->y +1;
			that->x4=that->x +1;
			that->y4=that->y +1;
		}
		if(that->direction==3)				//	  0
		{						//	000
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x -1;
			that->y2=that->y;
			that->x3=that->x +1;
			that->y3=that->y;
			that->x4=that->x +1;
			that->y4=that->y -1;
		}
	}
	if(that->type==6)					//	00
	{							//	00
			that->x1=that->x;
			that->y1=that->y;
			that->x2=that->x +1;
			that->y2=that->y;
			that->x3=that->x;
			that->y3=that->y +1;
			that->x4=that->x +1;
			that->y4=that->y +1;
	}
}


int check(unsigned int* table,structure* that)
{
	//没和已有的重合    -->    下移一格
	int check=0;
	unsigned int temp;

	if(that->x1<0 | that->x2<0 | that->x3<0 | that->x4<0) check=1;
	if(that->x1>31 | that->x2>31 | that->x3>31 | that->x4>31) check=1;

	temp=(table[that->y1]) & ( (unsigned int)1 << that->x1);
	if(temp != 0)	check=1;
	temp=(table[that->y2]) & ( (unsigned int)1 << that->x2);
	if(temp != 0)	check=1;
	temp=(table[that->y3]) & ( (unsigned int)1 << that->x3);
	if(temp != 0)	check=1;
	temp=(table[that->y4]) & ( (unsigned int)1 << that->x4);
	if(temp != 0)	check=1;

	return check;
}

void init()
{
	int i,j;

	for(i=0;i<48;i++) table[i]=0;
	table[48]=0xffffffff;
	that.x=random() %27 +1;
	that.y=1;
	that.type=5;
	that.direction=2;
	generate(&that);
}
void printworld()
{
	int i,j;
	//print world
	for(j=0;j<48;j++)
	{
		unsigned temp=table[j];
		for(i=0;i<32;i++)
		{
			cubie(i,j,temp & 0x1);
			temp/=2;    //temp>>1 wrong,do not know why
		}
	}
	//print cubies
	cubie(that.x1,that.y1,1);
	cubie(that.x2,that.y2,1);
	cubie(that.x3,that.y3,1);
	cubie(that.x4,that.y4,1);
	//print score
	//decimal(10,10,score);
	
	writescreen();
}
void left()
{
	if(that.x1>0&&that.x2>0&&that.x3>0&&that.x4>0)
	{
		that.x --;
		that.x1 --;
		that.x2 --;
		that.x3 --;
		that.x4 --;
		if(check(table,&that) != 0)
		{
			that.x ++;
			that.x1 ++;
			that.x2 ++;
			that.x3 ++;
			that.x4 ++;
		}
	}
}
void right()
{
	if(that.x1<31&&that.x2<31&&that.x3<31&&that.x4<31)
	{
		that.x ++;
		that.x1 ++;
		that.x2 ++;
		that.x3 ++;
		that.x4 ++;
		if(check(table,&that) != 0)
		{
			that.x --;
			that.x1 --;
			that.x2 --;
			that.x3 --;
			that.x4 --;
		}
	}
}
void up()
{
	that.direction=(that.direction +1)%4;
	generate(&that);
	if(check(table,&that) != 0)
	{
		that.direction=(that.direction+3)%4;
	}
	generate(&that);
}
void ticktock()
{
	that.y ++;
	that.y1 ++;
	that.y2 ++;
	that.y3 ++;
	that.y4 ++;
	//不能下移
	if(check(table,&that) != 0)
	{
		that.y --;
		that.y1 --;
		that.y2 --;
		that.y3 --;
		that.y4 --;
		//老的方块融入世界
		table[that.y1] |= ( (unsigned int)1 << that.x1);
		table[that.y2] |= ( (unsigned int)1 << that.x2);
		table[that.y3] |= ( (unsigned int)1 << that.x3);
		table[that.y4] |= ( (unsigned int)1 << that.x4);
		//新的方块们
		that.x=random() %27+1;
		that.y=1;
		that.type=random() % 7;
		that.direction=random() & 0x3;
		generate(&that);
	}
}
void down()
{
	ticktock();
}
void change()
{
	int i,j;
	for(i=47;i>0;i--)
	{
		if(table[i] == 0xffffffff)
		{
			for(j=i;j>0;j--)
			{
				table[j]=table[j-1];
			}
			table[0]=0;
			score++;
		}
	}
}
void main()
{
	//
	init();

	while(1)
	{
		//
		printworld();

		//wait
		int key=waitevent();

		//
		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 0x40000050:		//left
			{
				left();
				break;
			}
			case 0x4000004f:		//right
			{
				right();
				break;
			}
			case 0x40000052:		//up
			{
				up();
				break;
			}
			case 0x40000051:		//down
			{
				down();
				break;
			}
			case 0xff:
			{
				ticktock();
				break;
			}
		}
		change();
	}

}

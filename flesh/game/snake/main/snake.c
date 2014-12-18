typedef struct qwer{
	int x;
	int y;
}qwer;
typedef struct table{
	int x;
	int y;
	int direction;
}table;
static qwer food;
static table snake[100];
static int direction=4;



void cubie(x,y,color)
{
	int i,j;
	for(i=16*x;i<16*(x+1);i++)
		for(j=16*y;j<16*(y+1);j++)
			point(i,j,color);

	for(i=16*x;i<16*(x+1);i++){
		point(i,16*y,0);
		point(i,16*(y+1),0);
	}
	for(j=16*y;j<16*(y+1);j++){
		point(16*x,j,0);
		point(16*(y+1),j,0);
	}
}


void init()
{
	int i,j;

	//init food and snake 
	food.x=random() & 0x3f;
	food.y=random() % 48;
	snake[0].x=40;
	snake[0].y=22;
	snake[0].direction=1;
	snake[1].x=41;
	snake[1].y=22;
	snake[1].direction=1;
	for(i=2;i<100;i++) snake[i].direction=0;

	//init screen
	for(i=0;i<1024;i++)		//clear screen
		for(j=0;j<768;j++)
			point(i,j,0);
}
void printworld()
{
	int i=0;
	while( snake[i].direction != 0 )
	{
		cubie(snake[i].x,snake[i].y,0xffffffff);
		i++;
	}
	cubie(food.x,food.y,0xffffffff);
	writescreen();
}
int change()
{
	int i,j;
	//clear snake
	i=0;
	while( snake[i].direction != 0 )
	{
		cubie(snake[i].x,snake[i].y,0);
		i++;
	}

	//蛇头动，蛇身未动
	snake[0].direction=direction;
	switch(direction)
	{
		case 1: snake[0].x--; break;
		case 2: snake[0].x++; break;
		case 3: snake[0].y--; break;
		case 4: snake[0].y++; break;
	}

	//计算蛇身长度
	int count=0;
	while(snake[count].direction != 0)	count++;

	//撞墙或者撞自己
	if(snake[0].x>=64 | snake[0].x<0) return -1;
	if(snake[0].y>=48 | snake[0].y<0) return -1;
	for(i=1;i<count;i++)
	{
		if(snake[0].x==snake[i].x && snake[0].y==snake[i].y)
		{
			break;
		}
	}
	if(i!=count) return -1;

	//吃食物
	if(snake[0].x==food.x && snake[0].y==food.y)
	{
		if(count==100) return -1;
		food.x=random() & 0x3f;		//new food
		food.y=random() % 0x30;
		snake[count].x=snake[count-1].x;	//longer body
		snake[count].y=snake[count-1].y;
		snake[count].direction=snake[count-1].direction;
	}

	//蛇身动
	for(i=count-1;i>0;i--)
	{
		switch(snake[i].direction)
		{
			case 0:break;
			case 1:
			{
				snake[i].x--;
				snake[i].direction=snake[i-1].direction;
				break;
			}
			case 2:
			{
				snake[i].x++;
				snake[i].direction=snake[i-1].direction;
				break;
			}
			case 3:
			{
				snake[i].y--;
				snake[i].direction=snake[i-1].direction;
				break;
			}
			case 4:
			{
				snake[i].y++;
				snake[i].direction=snake[i-1].direction;
				break;
			}
		}
	}
}
void main()
{
	//
	init();

	while(1)
	{
		//print snake
		printworld();

		//wait
		int key=waitevent();
		
		//process it
		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 0x40000050:		//left
			{
				direction=1;
				break;
			}
			case 0x4000004f:		//right
			{
				direction=2;
				break;
			}
			case 0x40000052:		//up
			{
				direction=3;
				break;
			}
			case 0x40000051:		//down
			{
				direction=4;
				break;
			}
			case 0xff:
			{
				direction=snake[0].direction;
				break;
			}
		}
		if( change() <= 0 ) return;
	}

}

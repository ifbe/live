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
            if(temp!=0){
                point(x+j,y+i,0xffffffff);
            }

            /*
            else{
                point(x+j,y+i,0);
            }
            */

        }
    points++;
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
	if(key<0) key=-key;
        return key;
}


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


void main()
{
	int i,j;
	int direction=4;
	struct qwer{
		int x;
		int y;
	}food;
	struct table{
		int x;
		int y;
		int direction;
	}snake[100];




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

	//init timer
	int20();

	while(1)
	{
		//print snake
		i=0;
		while( snake[i].direction != 0 )
		{
			cubie(snake[i].x,snake[i].y,0xffffffff);
			i++;
		}
		cubie(food.x,food.y,0xffffffff);




		//waitforsometime
		//and change snake
		unsigned char key=hltwait();
		if(key==0x1) break;
		if(key==0x4b) direction=1;
		else if(key==0x4d) direction=2;
		else if(key==0x48) direction=3;
		else if(key==0x50) direction=4;
		else if(key==0xff) direction=snake[0].direction;
		else continue;


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
		if(snake[0].x>=64 | snake[0].x<0) break;
		if(snake[0].y>=48 | snake[0].y<0) break;
		for(i=1;i<count;i++)
		{
			if(snake[0].x==snake[i].x && snake[0].y==snake[i].y)
			{
				break;
			}
		}
		if(i!=count) break;


		//吃食物
		if(snake[0].x==food.x && snake[0].y==food.y)
		{
			if(count==100) break;
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
			case 1:{snake[i].x--;
				snake[i].direction=snake[i-1].direction;
				break;
			}
			case 2:{snake[i].x++;
				snake[i].direction=snake[i-1].direction;
				break;
			}
			case 3:{snake[i].y--;
				snake[i].direction=snake[i-1].direction;
				break;
			}
			case 4:{snake[i].y++;
				snake[i].direction=snake[i-1].direction;
				break;
			}
		}
		}


		//开始下一轮循环
	}
}

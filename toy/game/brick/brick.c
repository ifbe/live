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


void main()
{
	int i,j;
	int position=512;
	unsigned long long table[24];
	struct table{
		int x;
		int y;
		int dx;
		int dy;
	}ball;


	for(i=0;i<24;i++) table[i]=0xffffffffffffffff;
	ball.x=512;
	ball.y=684;
	ball.dx=random() & 0xf;
	ball.dy=16;


	//init screen
	for(i=0;i<1024;i++)		//clear screen
		for(j=0;j<768;j++)
			point(i,j,0);

	//init timer
	int20();

	while(1)
	{
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


		//waitforsometime
		unsigned char key=hltwait();

		//clear board
		for(i=position-64;i<position+64;i++)
		{
			point(i,700,0);
		}

		//clear ball
		cubie(ball.x,ball.y,0);

		//move board
		if(key==0x01) break;
		else if(key==0x4b){
			if(position>80)
			{
				position-=0x16;
			}
		}
		else if(key==0x4d){
			if(position<1024-80)
			{
				position+=0x16;
			}
		}
		else if(key==0xff)
		{
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
				temp=(unsigned long long)0x1<<bit;
				temp|=(unsigned long long)0x1<<(bit+1);
				temp=~temp;
				table[line]&=temp;
			}
		}

		//开始下一轮循环
		if(ball.y>750) break;
	}

	shutup20();
}

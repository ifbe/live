static int x=-250,y=0,xx=0x13,yy=0x8;


void cubie(int x,int y,int z)
{
    int i,j;
    for(i=-5;i<5;i++)
        for(j=-5;j<5;j++)
            point(x+512+i,384-y+j,z);
}
void move()
{
    cubie(x,y,0xff00);
    x+=xx;
    y+=yy;
    if((y>360)|(y<-360))
    {
        yy=-yy;
    }
    if((x>-10)|(x<-480))
    {
        xx=-xx;
    }
    cubie(x,y,0);
	writescreen();
}

void main()
{
    while(1)
    {
		int input=pollevent();
		if(input<=0) break;

		int i;
		for(i=0;i<10000000;i++);
		move();
    }
}


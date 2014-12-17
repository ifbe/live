static int x=250,y=0,xx=0x3,yy=0x7;


void move()
{
    point(x+512,384-y,0xff00);
    x+=xx;
    y+=yy;
    if((y>360)|(y<-360))
    {
        yy=-yy;
    }
    if((x<10)|(x>500))
    {
        xx=-xx;
    }
    point(x+512,384-y,0);
	writescreen();
}


void main()
{
    while(1)
    {
	int input=pollevent();
	if(input<=0) return;
	if(input==0x1b) return;

	int i;
	for(i=0;i<8000000;i++) asm("nop");
	move();
    }
}

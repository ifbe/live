#define u64 long long
double cosine(double x);
double sine(double x);
static double degree,arc=0;
static int table[1024];



void line(int x1,int y1,int x2,int y2)
{
	int x,y;
	if(x1 == x2)
	{
		if(y1<y2)
		{
			for(y=y1;y<y2;y++)
			{
				draw(x1,y,0xffffffff);
			}
		}
		if(y1>y2)
		{
			for(y=y2;y<y1;y++)
			{
				draw(x1,y,0xffffffff);
			}
		}
	}
	else
	{
		double k=(double)(y1-y2) / (double)(x1-x2);
		//if(x1<x2) x=x1;
		//if(x1>x2) x=x2;

		for(x=-500;x<500;x++)
		{
			y=y1+(int)(k*(double)(x-x1));
			if(y<380&&y>-380)draw(x,y,0xffffffff);
		}
	}
}


void printworld()
{
	int i,j;
	//清屏
	for(i=-512;i<512;i++)
		for(j=-384;j<384;j++)
			draw(i,j,0x44444444);

	//最上面横线
	line(-512,100,512,100);
	//圆
	for(degree=0;degree<6.28;degree+=0.01)
		draw((int)(100*cosine(degree)),(int)(100*sine(degree)),0xffffffff);
	//在圆上选一点
	int ax=(int)(100* cosine(arc) );
	int ay=(int)(100* sine(arc) );
	//过这一点的横线
	line(500,ay,ax,ay);
	//从圆最下面一点连线到选择的这一点上
	line(0,-100,ax,ay);
	//垂点横坐标
	int nx=(int)( 200*(double)ax / (100+ (double)ay) );
	//引垂线
	line(nx,ay,nx,100);
	//记录此横坐标对应的曲线点的纵坐标
	if(nx>0&&nx<512)table[nx]=ay;
	else if(nx<0&&nx>-512)table[1024+nx]=ay;

	for(i=0;i<1024;i++)
	{
		if(table[i]!=0) draw(i,table[i],0xffffffff);
	}
	
	writescreen();
}
void main()
{
	int i;
	for(i=0;i<1024;i++) table[i]=0;

	while(1)
	{
		//
		printworld();
		//
		int key=waitevent();
		//
		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 0x40000050:
			{
				if(arc> -1.57) arc+=0.01;
				break;
			}
			case 0x4000004f:
			{
				if(arc<4.71) arc-=0.01;
				break;
			}
		}
	}
}

#define u64 long long
static int table[16][16];
static int red=0,green=0,blue=0;


void printworld()
{
	int x=0,y=0,color;
	for(x=0;x<256;x++)
	{
		for(y=0;y<256;y++)
		{
			color=red*65536+y*256+x;
			point(2*x,2*y,color);
			point(2*x,2*y+1,color);
			point(2*x+1,2*y,color);
			point(2*x+1,2*y+1,color);
		}
	}
	color=red*65536+green*256+blue;
	for(x=512;x<1024;x++)
		for(y=0;y<512;y++)
			point(x,y,color);

	point(2*blue,2*green,0xffffffff);
	point(2*blue+1,2*green,0xffffffff);
	point(2*blue,2*green+1,0xffffffff);
	point(2*blue+1,2*green+1,0xffffffff);
	hexadecimal(40,0,red*65536+green*256+blue);
	
	writescreen();
}


void main()
{
	int x,y;
	for(x=0;x<16;x++)
		for(y=0;y<16;y++)
			table[x][y]=0;

	while(1)
	{
		printworld();
		int key=waitevent();
		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case '-':	//-
			{
				if(red>0) red--;
				break;
			}
			case '+':	//+
			{
				if(red<255) red++;
				break;
			}
			case 0x40000052:		//up
			{
				if(green>0) green--;
				break;
			}
			case 0x40000051:		//down
			{
				if(green<255) green++;
				break;
			}
			case 0x40000050:		//left
			{
				if(blue>0) blue--;
				break;
			}
			case 0x4000004f:		//right
			{
				if(blue<255) blue++;
				break;
			}
		}
	}
}

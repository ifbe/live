//i need:
//coordinate(int x,int y,int z,unsigned int color);
//writecoordinate();
//lookat(worldx,worldy,worldz,eyex,eyey,eyez,eyedirection);


void init()
{
	int x,y,z;

	for(x=0;x<256;x++)
	{
		for(y=0;y<256;y++)
		{
			for(z=0;z<256;z++)
			{
				coordinate(x,y,z,0);
			}
		}
	}
	for(x=10;x<20;x++)
	{
		for(y=50;y<100;y++)
		{
			for(z=0;z<128;z++)
			{
				coordinate(x,y,z,0xffffffff);
			}
		}
	}
	for(x=0;x<256;x++)
	{
		coordinate(x,0,0,0xff);
	}
	for(y=0;y<256;y++)
	{
		coordinate(0,y,0,0xff00);
	}
	for(z=0;z<256;z++)
	{
		coordinate(0,0,z,0xff0000);
	}
	
	writecoordinate();
}
void main()
{
	int degree=0;
	init();
	while(1)
	{
		//
		moveeye(degree);
		//
		int key=waitevent();
		//
		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 0x40000050:		//left
			{
				degree=(degree+359)%360;
				break;
			}
			case 0x4000004f:		//right
			{
				degree=(degree+1)%360;
				break;
			}
		}
	}
}
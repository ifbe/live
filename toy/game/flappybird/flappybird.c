int random()
{
        int key,i=0;
        char* memory=(char*)0x0;
        for(i=0;i<0x1000;i++)
                key+=memory[i];
	if(key<0) key=-key;
        return key;
}


void main()
{
	int i,j;
	int x1,y1,x2,y2,x3,y3,x4,y4;
	for(i=0;i<1024;i++)
		for(j=0;j<768;j++)
			point(i,j,0);

	x1=100;y1=400;
	x2=400;y2=200;
	x3=700;y3=300;
	x4=1000;y4=500;

	int20();

	while(1)
	{
		//print world
		i=x1;
		if(i<=0)	i=0;
		for(;i<x1+100;i++)
		{
			for(j=0;j<y1-100;j++)
				point(i,j,0xff00);
			for(j=y1+100;j<768;j++)
				point(i,j,0xff00);
		}
		for(i=x2;i<x2+100;i++)
		{
			for(j=0;j<y2-100;j++)
				point(i,j,0xff00);
			for(j=y2+100;j<768;j++)
				point(i,j,0xff00);
		}
		for(i=x3;i<x3+100;i++)
		{
			for(j=0;j<y3-100;j++)
				point(i,j,0xff00);
			for(j=y3+100;j<768;j++)
				point(i,j,0xff00);
		}
		int temp=x4+100;
		if(temp>1024)	temp=1024;
		for(i=x4;i<temp;i++)
		{
			for(j=0;j<y4-100;j++)
				point(i,j,0xff00);
			for(j=y4+100;j<768;j++)
				point(i,j,0xff00);
		}

		//print bird

		//wait
		char key=hltwait();
		if(key==0x01) break;
		else if(key==0x39)
		{
		}
		else if(key== -1)
		{
			for(i=0;i<1024;i++){
				for(j=0;j<768;j++)
				{
					point(i,j,0);
				}
			}
	
			x1-=16;
			x2-=16;
			x3-=16;
			x4-=16;

			if(x1< -100){
				x1=x2;
				x2=x3;
				x3=x4;
				x4+=300;
				y1=y2;
				y2=y3;
				y3=y4;
				y4=random() %600;
			}
		}

		//one line full ?

		//next loop
	}

	shutup20();
}

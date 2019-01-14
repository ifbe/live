#define u64 long long


double cosine(double x)
{
    double ret=0,item=1.0,temp;
    int n=0,i,sign=1;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        temp=item;
        for(i=1;i<=2*n;i++)temp/=i;
        ret+=sign*temp;
        item*=x*x;
        sign *=-1;
        n++;
      }while (temp>1e-10);
return ret;
}


double sine(double x)
{
    int m = 1,i;
    double temp,ret = 0.0;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        i=0;
        if (m%2 == 0){temp= -1.0;}
        else{temp= 1.0;}
        for(i=1;i<=2*m-1;i++){temp = temp * x/i;}
        ret+= temp;
        m++;
    }while (temp<-.0000005||temp>0.0000005);
return ret;
}


void dot(int x,int y,int color)
{
	point(x-2,y,color);

	point(x-1,y-1,color);
	point(x-1,y,color);
	point(x-1,y+1,color);

	point(x,y-2,color);
	point(x,y-1,color);
	point(x,y,color);
	point(x,y+1,color);
	point(x,y+2,color);

	point(x+1,y-1,color);
	point(x+1,y,color);
	point(x+1,y+1,color);

	point(x+2,y,color);
}


void main()
{
	int positionx,positiony;
	int x0,y0,x1,y1,x2,y2,x3,y3,x4,y4;
	int i,j;
	double angle=0;

	for(i=0;i<1024;i++)
		for(j=0;j<768;j++)
			point(i,j,0);
	positionx=512;
	positiony=600;
	x0=positionx+16*cosine(angle);
	y0=positiony+16*sine(angle);
	x1=positionx+16*cosine(angle+1);
	y1=positiony+16*sine(angle+1);
	x2=positionx+16*cosine(angle+2);
	y2=positiony+16*sine(angle+2);
	x3=positionx+16*cosine(angle+3);
	y3=positiony+16*sine(angle+3);
	x4=positionx+16*cosine(angle+4);
	y4=positiony+16*sine(angle+4);

	for(i=500-40;i<500+75;i++)
	{
		for(j=300-40-(i-500)/9;j<300+40+(i-500)/9;j++)
		{
			point(i,j,0xc0ff);
		}
	}
	for(j=200;j<400;j++)
	{
		point(512,j,0);
		point(513,j,0);
		point(514,j,0);
		point(515,j,0);
	}
	for(i=500-400;i<500+75;i++)
	{
		point(i,298,0);
		point(i,299,0);
		point(i,300,0);
		point(i,301,0);
	}

	while(1)
	{
		//print world
		for(i=positionx-32;i<positionx+32;i++)
			for(j=positiony-32;j<positiony+32;j++)
				point(i,j,0);
		dot(x0,y0,0xffffffff);
		dot(x1,y1,0xffffffff);
		dot(x2,y2,0xffffffff);
		dot(x3,y3,0xffffffff);
		dot(x4,y4,0xffffffff);
		writescreen();

		//wait
		int key=waitevent();

		switch(key)
		{
			case -1:return;
			case 0x1b:return;
			case 0xff:
			{
				angle+=0.2;
				x0=positionx+16*cosine(angle);
				y0=positiony+16*sine(angle);
				x1=positionx+16*cosine(angle+1);
				y1=positiony+16*sine(angle+1);
				x2=positionx+16*cosine(angle+2);
				y2=positiony+16*sine(angle+2);
				x3=positionx+16*cosine(angle+3);
				y3=positiony+16*sine(angle+3);
				x4=positionx+16*cosine(angle+4);
				y4=positiony+16*sine(angle+4);
			}
		}
	}
}

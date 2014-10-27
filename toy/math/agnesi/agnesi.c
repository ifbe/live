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


void point(int x,int y,int z)
{
    u64* video=(u64*)0x3028;
    u64 base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+( (384-y) *1024+ (x+512) )*bpp);
    *address=z;
}


void line(int x1,int y1,int x2,int y2)
{
	int x,y;
	if(x1 == x2){
		if(y1<y2){
		for(y=y1;y<y2;y++)
		{
			point(x1,y,0xffffffff);
		}
		}
		if(y1>y2){
		for(y=y2;y<y1;y++)
		{
			point(x1,y,0xffffffff);
		}
		}
	}

	else
	{
		double k=(double)(y1-y2) / (double)(x1-x2);
		if(x1<x2) x=x1;
		if(x1>x2) x=x2;

		for(x= -500;x<500;x++)
		{
			y=y1+ (double)(x-x1)*k;
			if(y<380&&y> -380)
				point(x,y,0xffffffff);
		}
	}
}


void main()
{
	char key;
	double degree,arc=0;
	int i,j;
	int table[1024];

	for(i=0;i<1024;i++) table[i]=0;

	while(1)
	{
		for(i=-512;i<512;i++)
			for(j= -384;j<384;j++)
				point(i,j,0x11111111);

		for(degree= 0;degree<6.28;degree+=0.01)
			point(100*cosine(degree),100*sine(degree),0xffffffff);

		line(-512,100,512,100);
		int ax=(int)(100* cosine(arc) );
		int ay=(int)(100* sine(arc) );
		line(500,ay,ax,ay);
		line(0,-100,ax,ay);
		int nx=(int)( 200*(double)ax / (100+ (double)ay) );
		line(nx,ay,nx,100);
		table[nx]=ay;
		for(i=0;i<1024;i++)
		{
		if(table[i]!=0) point(i,table[i],0xffffffff);
		}

		key=hltwait();
		if(key==0x4d){
			if(arc> -1.57) arc-=0.01;
		}
		if(key==0x4b){
			if(arc<4.71) arc+=0.01;
		}
	}
}

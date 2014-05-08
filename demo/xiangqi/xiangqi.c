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


void background()
{
	int i,j,k;
	for(i=20;i<1000;i++)
		for(j=20;j<750;j++)
			point(i,j,0xeeee55);	//background

	for(i=200;i<760;i++)
		for(j=350;j<420;j++)
			point(i,j,0x999922);	//background

	for(j=70;j<=700;j+=70)
		for(i=200;i<760;i++)
			point(i,j,0);		//heng

	for(i=200;i<830;i+=70)
		for(j=70;j<=700;j++)
			point(i,j,0);		//shu

	i=410;
	j=70;
	for(k=0;k<140;k++)
	{
		point(i+k,j+k,0);
		point(i+k,j+k+490,0);
		point(i+k,j+140-k,0);
		point(i+k,j+140-k+490,0);
	}
}

void chess(int x,int y,int z)
{
	int i,j;
	x=200+(x-1)*70;
	y=y*70;
	for(i=-20;i<20;i++)
		for(j=-20;j<20;j++)
			point(x+i,768-y+j,z);
}

void main()
{
	int x,y;

	background();

	for(x=1;x<=9;x++)
		for(y=1;y<=10;y++)
			chess(x,y,0xffffffff);	//qizi
}

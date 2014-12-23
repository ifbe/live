#define u64 long long
double cosine(double x);
double sine(double x);


void generate()
{
int* p;
int x,y,z;

//[64M,128M],原点：0x6000000,4B*0x100*0x100*0x100=0x4000000=64M
for(p=(int*)0x400000;p<(int*)0x800000;p++){*p=0x0;}

for(x=-128;x<0;x++) *(int*)(u64)(0x6000000+x*4)=(int)(256+x);
for(x=0;x<128;x++) *(int*)(u64)(0x6000000+x*4)=(int)x;

for(y=-128;y<0;y++) *(int*)(u64)(0x6000000+y*4*256)=((int)(256+y))<<8;
for(y=0;y<128;y++) *(int*)(u64)(0x6000000+y*4*256)=((int)y)<<8;

for(z=-128;z<0;z++) *(int*)(u64)(0x6000000+z*4*256*256)=((int)(256+z))<<16;
for(z=0;z<128;z++) *(int*)(u64)(0x6000000+z*4*256*256)=((int)z)<<16;

for(x=25;x<50;x++)
{
 for(y=-10;y<10;y++)
 {
  for(z=-20;z<20;z++)
  {
	*(int*)(u64)(0x6000000+4*x+4*y*256+4*z*256*256)=0xffffff;
  }
 }
}

}


void display()
{
	int u,v,w;	//眼中坐标
	int x,y,z;	//世界坐标
	int color;
	int* world;
	double cd=cosine(d);
	double ce=cosine(e);
	double cf=cosine(f);
	double sd=sine(d);
	double se=sine(e);
	double sf=sine(f);

	for(v=-128;v<128;v++)
	{
		for(u=-128;u<128;u++)
		{
			color=0;
			for(w=-128;w<128;w++)
			{
			x=(int)(ce*cf*(double)u+(sd*se*cf-cd*sf)*(double)v+(sd*sf+cd*se*cf)*(double)w);

			y=(int)(ce*sf*(double)u+(cd*cf+sd*se*sf)*(double)v+(cd*se*sf-sd*cf)*(double)w);

			z=(int)(se*(double)u+sd*ce*(double)v+cd*ce*(double)w);

			if(x>-128&&x<128&&y>-128&&y<128&&z>-128&&z<128)
			{
			world=(int*)(u64)((z*256*256+y*256+x)*4+0x6000000);
			color+=*world;
			}


			}
			point(u+512,384-v,color);
		}
	}
}

void main()
{
	char in=0;

	generate();

	while(1)
	{
		display();
		in=hltwait();
		if(in==0x01) break;
		else if(in==0x48)d=d+0.01;
		else if(in==0x50)d=d-0.01;
		else if(in==0x4d)e=e-0.01;
		else if(in==0x4b)e=e+0.01;
		else if(in==0x33)f=f-0.01;
		else if(in==0x34)f=f+0.01;
	}
}


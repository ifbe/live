/*
void point(int x,int y,int z)
{
    long long* video=(long long*)0x3028;
    long long base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=z;
}
*/


void point(int x,int y,int color)
{
	unsigned int* mypixel=(unsigned int*)0x1400000;
	mypixel[y*1024+x]=color;
}


void writescreen()
{
    unsigned long long base=*(unsigned long long*)0x3028;
    char bpp=(*(char*)0x3019)/8;

	unsigned long long i;
	for(i=0;i<1024*768;i++)
	{
		*(unsigned int*)(base+bpp*i)=*(unsigned int*)(0x1400000+4*i);
	}
}
//#include<stdio.h>
unsigned long long fileaddr();
int filesize();

int main()
{
	unsigned long long addr=fileaddr();
	int size=filesize();

	int i=njDecode((void*)addr,size);
	if(i!=0) return i;

	int w,h,x,y;
	unsigned long long rgb;
	getpicture(&rgb,&w,&h);

	unsigned char* p=(unsigned char*)rgb;
	for(y=0;y<768;y++)
	{
		for(x=0;x<1024;x++)
		{
			point(x,y,(p[0]<<16)+(p[1]<<8)+p[2]);
			p+=3;
		}
	}
	writescreen();

	while(waitevent()>0);

	return 0;
}

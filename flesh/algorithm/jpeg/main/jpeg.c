extern unsigned char jpegstart;
extern unsigned char jpegend;


void main()
{
	printf("jpeg@%x\nsize:%x\n",&jpegstart,&jpegend-&jpegstart);


	int i=njDecode(&jpegstart,&jpegend-&jpegstart);
	if(i!=0)
	{
		printf("error:%d\n",i);
		return;
	}

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
}

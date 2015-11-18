#define u64 long long
double cosine(double x);
double sine(double x);


void main()
{
    double angle=3.1415*2;
    int x,y; 

    for(x=0;x<1024;x++)
    {
		for(y=0;y<768;y++)
		{
			point(x,y,0x44444444);
		}
    }

    while((angle-=0.001)>0)
    {
        x=(int)(250*cosine(angle));
        y=(int)(250*sine(angle));
        draw(x,y,0x000000);
    }
	writescreen();

    if(waitevent() <= 0)return;
}

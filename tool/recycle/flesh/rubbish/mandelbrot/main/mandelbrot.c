#define u64 long long

void init()
{
        int x,y;
        for(x=0;x<1024;x++)
        {
                for(y=0;y<768;y++)
                {
                        point(x,y,0x44444444);
                }
        }
}


double squareroot(double x)
{
    double temp,ret;
    if(x<0){return 0;}

    temp=0.5*(1+x);
    ret=0.5*(temp+x/temp);
    while(1)
    {
	if( (ret-temp) >= 1e-10 ) break;
	if( (temp-ret) >= 1e-10 ) break;
        temp=ret;
        ret=0.5*(temp+x/temp);
    }
return ret;
}


void compute(double* ci,double* cj)
{
	double temp1=*ci;
	double temp2=*cj;
	*ci=temp1*temp1-temp2*temp2+0.03;
	*cj=2*temp1*temp2+0.7;
}


void main()
{
	init();

	double ci=0.03,cj=0.7;
	int x=0,y=0;
	while(x<1000)
	{
		printdouble(0,0,ci);
		printdouble(0,1,cj);

		compute(&ci,&cj);
		y=squareroot(ci*ci+cj*cj);
		if(y>0 && y<350)
		{
			point(x,y,0xffcc00);
		}
		else break;

		x++;
	}
}

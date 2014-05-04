#define u64 long long
void main()
{
    double base=0.0;
    while(1)
    {
        taiji(&base);
    }
}
void taiji(double* base)
{
    double angle;
    int basex,basey,x,y;
    *base+=0.01;
    basex=(int)(100*cosine(*base));
    basey=(int)(100*sine(*base));
    for(angle=0;angle<3.14;angle+=0.01)
    {
        x=basex+(int)(100*cosine(*base+angle));
        y=basey+(int)(100*sine(*base+angle));
        draw(x+250,y,0xffffffff);
        draw(-x+250,-y,0);
    }
}


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

    address=(int*)(base+(y*1024+x)*bpp);
    *address=z;
}


void draw(int x,int y,int z)
{
point(x+512,384-y,z);
}

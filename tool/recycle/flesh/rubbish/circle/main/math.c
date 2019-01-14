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


double absolute(double ret)
{
    return ret>=0?ret:-ret;
}


double squareroot(double x)
{
    double temp,ret;
    if(x<0){return 0;}

    temp=0.5*(1+x);
    ret=0.5*(temp+x/temp);
    while(absolute(ret-temp)>=1e-10)
    {
        temp=ret;
        ret=0.5*(temp+x/temp);
    }
return ret;
}


double ln(double a)
{
    int N = 15;//我们取了前15+1项来估算
    int k,nk;
    double x,xx,y;
    x = (a-1)/(a+1);
    xx = x*x;
    nk = 2*N+1;
    y = 1.0/nk;
    for(k=N;k>0;k--)
    {
        nk = nk - 2;
        y = 1.0/nk+xx*y;
    }
    return 2.0*x*y;
}


double logarithm(double x,double y)
{
    return ln(y)/ln(x);
}


double exponent(double x)
{
    double temp,ret = 1.0;
    int n = 1,i = 0;
    do{
        temp=1.0;
        for (i=1;i<=n;i++){temp = temp*x/i;}
        ret += temp;
        n++;
    }while(n<20);
return ret;
}


double power(double x,double y)
{
    return exponent(y*ln(x));
}


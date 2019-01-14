void taiji()
{
    double x,y,a;
    for(a=-3.1415/2;a<3.1415/2;a+=0.01)    //右月牙
    {
        x=cosine(a);
        y=sine(a);
        point((short)(100*x)+50,(short)(100*y),0x02);
        point((short)(50*x)+50,(short)(50*y)-50,0x06);
        point(-(short)(50*x)+50,50-(short)(50*y),0x06);
    }
    point(50,50,0x07);

    for(a=3.1415/2;a<3.1415*1.5;a+=0.01)    //左月牙
    {
        x=cosine(a);
        y=sine(a);
        point((short)(100*x)-50,(short)(100*y),0x01);
        point((short)(50*x)-50,(short)(50*y)+50,0x05);
        point(-(short)(50*x)-50,-(short)(50*y)-50,0x05);
    }
    point(-50,-50,0x07);        //
}

void main()                //位置固定在第一个函数
{
    taiji();               //太极
}
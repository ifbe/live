double sine(double x);
double cosine(double x);


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


void geometry(char* anscii)
{
    int i;
    double x;
    for(i=-500;i<500;i++){draw(i,0,0);}    //横坐标
    for(i=-300;i<300;i++){draw(0,i,0);}    //纵坐标
                             //下面描点
    if(*anscii=='s')
    {
//string(0,1,"???????????????");
        for(x=-5.00;x<5.00;x+=0.01)
        {
            draw((int)(100*x),(int)(100*sine(x)),0xffffffff);
        }
    }
    if(*anscii=='c')
    {
        for(x=-5.00;x<5.00;x+=0.01)
        {
            draw((int)(100*x),(int)(100*cosine(x)),0xffffffff);
        }
    }
}


void analyse(char* anscii)
{
    int i=0;

    init();
    string(0,0,anscii);
    geometry(anscii);
}


void main()
{
    unsigned char buffer[128];
    unsigned char anscii[128];
    int i;
    anscii[127]='\0';        //字符串结尾
    for(i=0;i<127;i++){anscii[i]=0x20;}
    i=0;

    init();
    string(55,20,"geometry sketchpad");

    while(1)
    {
        buffer[i]=hltwait();
	if(buffer[i]>=0x80) continue;

        switch(buffer[i])
        {
            case 0x01:return;
            case 0x0e:{
                print(i-1,0,0x20);
                anscii[i]=0x20;
                if(i!=0)i--;
                break;
            }
            case 0x1c:{
                analyse(anscii);           //进行计算
                for(i=0;i<127;i++){anscii[i]=0x20;} //处理完毕，清空
                i=0;                           //从头再来
                break;
            }
            default:{
                anscii[i]=convert(buffer[i]);  //进来一个convert一个
                string(0,0,anscii);
                i=(i+1)%127;
            }
        }
    }
}

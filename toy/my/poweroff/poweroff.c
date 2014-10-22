void turnoff()
{
    short* p;
    short port,val;
    p=(short*)0x4fc;
    port=*p;
    p=(short*)0x4fe;
    val=(*p)|0x2000;
    outw(port,val);
}

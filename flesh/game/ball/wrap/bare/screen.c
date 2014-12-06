void point(int x,int y,int z)
{
    unsigned long long base=*(unsigned long long*)0x3028;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=z;
}
writescreen(){}
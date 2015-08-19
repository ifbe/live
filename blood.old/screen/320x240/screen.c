void point(short x,short y,char z)
{
	//0xa0000~0xa7aff,0xa7d00~0xaf9ff
    *(char*)(unsigned long long)(0xa7c60+y*320+x)=z;
}
void writescreen(){}
#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long




//学会一个函数(把地址扔进/bin)
void remember(QWORD name,QWORD addr)
{
        QWORD* pointer=(QWORD*)0x180000;
        int i;
        for(i=0;i<0x200;i+=2)
        {
        if( (pointer[i]==0) | (pointer[i]==name) )
        {
                pointer[i]=name;
                pointer[i+1]=addr;
                break;
        }
        }
}




//使用刚学会的函数(从/bin找到地址并执行)
int use(QWORD funcname,QWORD arg1)
{
        QWORD* pointer=(QWORD*)0x180000;
        int i;
	int (*func)(QWORD arg);
	int ret;

        for(i=0;i<0x200;i+=2)
        {
        if(pointer[i]==funcname)
        {
		func=(int (*)(QWORD))(pointer[i+1]);
                break;
        }
        }
	if(i==0x200) return -1;		//没找到，滚

	return func(arg1);
}

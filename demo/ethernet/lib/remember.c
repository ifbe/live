#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long




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

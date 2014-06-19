#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


void blank2zero(QWORD* name)
{
        int i;
	QWORD temp;
        for(i=0;i<8;i++)
        {
                temp = ( (*name)>>(i*8) )&0xff;
		if(temp == 0x20)
                {
                        *name -= (QWORD)0x20<<(i*8);
                }
        }

}


void small2capital(QWORD* name)
{
        int i;
	QWORD temp;
        for(i=0;i<8;i++)
        {
                temp = ( (*name) >> (i*8) )&0xff;
                if(temp>='a' && temp<='z')
                {
                        *name -= ( (QWORD)0x20 )<<(i*8);
                }
        }
}




QWORD getdisk()
{
        int i;
        QWORD* pointer=(QWORD*)0x4000;
        for(i=0;i<0x200;i+=2)
        {
                if( pointer[i] == 0x2020202061746173 )
                {
                        return pointer[i+1];
                }
        }

        return 0;               //only when there is no sata
}

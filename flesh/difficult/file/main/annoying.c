#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


//����aa zec���õ�aa\0ec
void blank2zero(QWORD* name)
{
        int i;
	QWORD temp;
        for(i=0;i<8;i++)
        {
                temp = ( (*name)>>(i*8) )&0xff;
		if(temp == 0x20)
                {
                        *name -= ((QWORD)0x20)<<(i*8);
                }
        }

}
//����ac\0be���õ�ac be
void zero2blank(QWORD* name)
{
        int i;
	QWORD temp;
        for(i=0;i<8;i++)
        {
                temp = ( (*name)>>(i*8) )&0xff;
		if(temp == 0)
                {
                        *name += ((QWORD)0x20)<<(i*8);
                }
        }

}
//����abcefg�����ABCEFG
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
//����0x36��0x38,0x37,0x39,ת����õ�data=0x39373836
void str2data(BYTE* str,QWORD* data)
{
	*data=0;
	int i;
	for(i=0;i<8;i++)
	{
		if(str[i]<=0x20) break;
		*data+=((QWORD)str[i])<<(i*8);
	}
}
//anscii��ת����һ�����֣�����anscii�봮Ϊ0x36,0x33,ת����õ�decimal=32
void anscii2dec(BYTE* second,QWORD* decimal)
{
	*decimal=0;
	int i;
	for(i=0;i<8;i++)
	{
		if(second[i]<=0x20) break;
		//say("%x\n",second[i]);
		*decimal=(second[i]-0x30)+(*decimal)*10;
	}
}
//���յ���ת����������buf�����ǡ�mount 12����ת����õ�firstΪ��mount����secondΪ��12��
void buf2arg(char* buffer,QWORD* first,QWORD* second)
{
	int i=0,j,count;
	char* temp;
	*first=*second=0;

	temp=(char*)first;
	for(;i<128;i++)
	{
		if(buffer[i]==0)return;
		if(buffer[i]<=0x20)continue;
		//say("buffer[%d]=%c\n",i,buffer[i]);
		for(count=0;count<8;count++)		//���ٸ���ĸ
		{
			if(buffer[i+count]<=0x20)break;
		}
		//say("%d\n",count);
		for(j=0;j<count;j++)
		{
			temp[j]=buffer[i+j];
		}
		//say(temp);
		i+=count;
		break;
	}

	temp=(char*)second;
	for(;i<128;i++)
	{
		if(buffer[i]==0)return;
		if(buffer[i]<=0x20)continue;
		//say("buffer[%d]=%c\n",i,buffer[i]);
		for(count=0;count<8;count++)		//���ٸ���ĸ
		{
			if(buffer[i+count]<=0x20)break;
		}
		//say("%d\n",count);
		for(j=0;j<count;j++)
		{
			temp[j]=buffer[i+j];
		}
		//say(temp);
		i+=count;
		break;
	}
}
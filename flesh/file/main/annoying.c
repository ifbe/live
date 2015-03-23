#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long




//------------------�ֲڵĴ�ȷ�--------------------
//һ����ׯ��ÿ���ļ���
//����Ǩ���ļ�ϵͳ����
//��������˼ң�����ļ���Щ�ֽ��ֽ��ֽ��ֽ��ֽ�...��
//��ɢ�����ֵط���Ӳ���Ϸ����ڵڼ��������ڼ�������
//�����������һ�غ��������ʵ�������Ķ�������ܹ����ٸ�����������߼������Ķ�����Ҫ�Ķ���
//���Ǹ��ϴ�����Ա˵������������ס�ģ��ܹ������ˣ�������Ǩǰס���ģ�Ҫ�Ҵ����ļ����˼ң�

//-------------------��ϸ�ĳ���---------------------
//20���׵ĳ��ӵ�3��11����Ϳ����ɫ������ͬ��С�����ϳɺü���
//�ֱ���[0,2],[2,4],[4,8],[8,10],[10,14],[14,20]
//����Ҫ��Ϳɫ�ĵط�����(�涨�ĵط�)��ûͿɫ�Ĳ����ӵ�����
//��Ϊ��ѳ���ÿ��������涼�п̶ȣ����Դ�0��ʼ�����źöӵ��Ÿ�
//[0,2],[14,20]:�����鶼��Ҫ����
//[2,4]:�г�[3,4],����(�涨�ĵط�),�ܹ�1cm
//[4,8]:ȫ������(�涨�ĵط�+1cm),�ܹ�4cm
//[8,10]:ȫ������(�涨�ĵط�+4cm),�ܹ�2cm
//[10,14]:�г�[10,11],����(�涨�ĵط�+7cm),�ܹ�1cm

//--------------------��ͼ˵��--------------------
//               |[wantwhere,wantwhere+1m]|
//1:             |                        | [where,]  //�Լ������ų�����Ҫ�鷳�������
//2: [where,]    |                        |           //�Լ������ų�����Ҫ�鷳�������
//3:         [---|--where,--]             |           //������һ�飬ǰ�治Ҫ
//4:         [---|--where,----------------|----]      //������һ�飬ǰ�治Ҫ������Ҳ��Ҫ
//5:             |  [where,]              |           //������һ�飬ȫҪ
//6:             |  [---where,------------|----]      //������һ�飬���治Ҫ

void holyshit(sector,count,where,wantwhere,towhere)
{
	//�ؼ�:�����Ķ������ĺ�����������������
	QWORD rdi=0;
	QWORD rsi=0;
	QWORD rcx=0;

	//��rdi,rsi,rcx��ֵ
	if(where<wantwhere)		//3��4
	{
		rdi=towhere;
		rsi=sector+(wantwhere-where)/0x200;
		if(where+count*0x200<=wantwhere+0x100000)
		{
			rcx=count-(wantwhere-where)/0x200;
		}
		else
		{
			rcx=0x100000/0x200;
		}
	}
	else
	{
		rdi=towhere+(where-wantwhere);
		rsi=sector;
		if(where+count*0x200<=wantwhere+0x100000)
		{
			rcx=count;
		}
		else
		{
			rcx=(wantwhere+0x100000-where)/0x200;
		}
	}

	say("sector:%x,count:%x,where:%x,want:%x,to:%x,rdi=%x,rsi=:%x,rcx=%x\n",sector,count,where,wantwhere,towhere,rdi,rsi,rcx);
	read(rdi,rsi,0,rcx);
}




int compare(unsigned char* first,unsigned char* second)
{
	int i;
	//say("%s,%s\n",first,second);
	for(i=0;i<16;i++)
	{
		if( (first[i]==0) && (second[i]==0) )return 0;		//�Ƚ���ϣ���ͬ
		if(first[i]!=second[i])return -1;
	}
}




//debug�ã���ӡ��addr��ʼ���ܹ�size���ֽ�
void printmemory(QWORD addr,QWORD size)
{
	BYTE* printaddr=(BYTE*)addr;
	int i,j;

	//1111111111111111111111
	say("[---addr---]");
	for(i=0;i<=0xf;i++)
	{
		say("%2x ",i);
	}
	say("[----anscii----]\n");

	//2222222222222222222222222
	for(j=0;j<size/16;j++)
	{
		say("%-12x",addr+j*16);
		for(i=0;i<=0xf;i++)
		{
			say("%2x ",printaddr[16*j+i]);
		}
		for(i=0;i<=0xf;i++)
		{
			unsigned char ch=printaddr[16*j+i];
			if( (ch>=0x80)|(ch<=0x20) )ch=0x20;
			say("%c",ch);
		}
		say("\n",printaddr[16*j+15]);
	}
}




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
//���յ���ת��������
//���buf�����ǡ�mount����ת����õ�firstΪ��mount����secondΪ0xffffffff
//���buf�����ǡ�mount 12����ת����õ�firstΪ��mount����secondΪ��12��
void buf2arg(char* buffer,QWORD* first,QWORD* second)
{
	int i=0,j,count;
	char* temp;

	//�յ�����յ����ֻ᷵��-1
	first[0]=first[1]=second[0]=second[1]=0xffffffffffffffff;

	temp=(char*)first;
	for(;i<128-16;i++)
	{
		//say("buffer[%d]=%c\n",i,buffer[i]);
		//�ַ�����β����0
		if(buffer[i]==0)return;
		//Խ�����з������ַ�
		if(buffer[i]<=0x20)continue;

		//����16���ֽڵ��������ְɣ������ٸ�
		for(count=0;count<16;count++)
		{
			if(buffer[i+count]<=0x20)break;
		}
		//say("%d\n",count);

		//������ַ��Ļ���first buffer�Ͳ�����-1(�ޱ�־)��
		if(count>0)first[0]=first[1]=0;
		for(j=0;j<count;j++)
		{
			temp[j]=buffer[i+j];
		}
		//say(temp);

		i+=count;
		break;
	}

	temp=(char*)second;
	for(;i<128-16;i++)
	{
		//say("buffer[%d]=%c\n",i,buffer[i]);
		if(buffer[i]==0)return;
		if(buffer[i]<=0x20)continue;

		//����16���ֽڵ��������ְɣ������ٸ�
		for(count=0;count<16;count++)
		{
			if(buffer[i+count]<=0x20)break;
		}
		//say("%d\n",count);

		//������ַ��Ļ���first buffer�Ͳ�����-1(�ޱ�־)��
		if(count>0)second[0]=second[1]=0;
		for(j=0;j<count;j++)
		{
			temp[j]=buffer[i+j];
		}
		//say(temp);

		i+=count;
		break;
	}
}
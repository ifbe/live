#define journalhome 0xd00000
#define journalsize 0x100000
#define QWORD unsigned long long
#define DWORD unsigned int




int hexadecimal(char* destaddr,QWORD data)
{
	QWORD temp;
	int count;
	int i;

	//检查多少个nibbie需要显示出来
	temp=data;
	count=0;
	while(1)
	{
		if(temp==0)break;

		temp=temp>>4;
		count++;
	}
	if(count==0)
	{
		*destaddr=0x30;
		return 1;
	}

	//显示数字
	for(i=0;i<count;i++)
	{
		temp=(data&0xf) + 0x30;
		if(temp>0x39)temp+=7;

		data=data>>4;
		destaddr[count-1-i]=temp;
	}

	return count;
}




int decimal(char* destaddr,QWORD data)
{
	QWORD temp;
	int count;
	int i;

	//检查多少个nibbie需要显示出来
	temp=data;
	count=0;
	while(1)
	{
		if(temp==0)break;

		temp=temp/10;
		count++;
	}
	if(count==0)
	{
		*destaddr=0x30;
		return 1;
	}

	//显示数字
	for(i=0;i<count;i++)
	{
		temp=(data%10) + 0x30;
		if(temp>0x39)temp+=7;

		data=data/10;
		destaddr[count-1-i]=temp;
	}

	return count;
}




//void say(char* p,...)




void diary(char* p,...)
{
	//保存传入的寄存器值
	register unsigned long long rsi asm("rsi");
	register unsigned long long rdx asm("rdx");
	register unsigned long long rcx asm("rcx");
	register unsigned long long r8 asm("r8");
	register unsigned long long r9 asm("r9");
	unsigned long long argtable[5]={rsi,rdx,rcx,r8,r9};
	int argcount=0;

	//这次往哪儿写（要确保不写到指定以外的地方）
	unsigned long long temp=*(QWORD*)(journalhome+journalsize-8);
	if(temp>=journalsize-0x40)
	{
		temp=0;
		*(QWORD*)(journalhome+journalsize-8)=0;
	}
	char* destaddr=(char*)(journalhome+temp);

	//把传入的字符串写进buffer里面
	int in=0;
	int out=0;
	while(1)
	{
		if(p[in] == '\0')break;		//是0，字符串结束了

		else if(p[in]==0x9)
		{
			*(DWORD*)(destaddr+out)=0x20202020;
			in++;
			out+=4;
		}
		else if(p[in]=='%')		//%d,%c,%lf,%llx.....
		{
			if(p[in+1]=='x')
			{
				in+=2;
				out+=hexadecimal(destaddr+out,argtable[argcount]);
				argcount++;
			}
			else if(p[in+1]=='d')
			{
				in+=2;
				out+=decimal(destaddr+out,argtable[argcount]);
				argcount++;
			}
			else
			{
				destaddr[out]=p[in];
				in++;
				out++;
			}
		}
		else				//normal
		{
			destaddr[out]=p[in];
			in++;
			out++;
		}

	}//while(1)finish

	*(QWORD*)(journalhome+journalsize-8)+=0x40;
}
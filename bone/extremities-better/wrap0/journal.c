#define journalhome 0xd00000
#define journalsize 0x100000
#define QWORD unsigned long long
#define DWORD unsigned int




void hexadecimal(char* destaddr,QWORD data)
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

	//显示数字
	temp=data;
	*(QWORD*)destaddr=0x2020202020202020;
	for(i=0;i<count;i++)
	{
		temp=(data&0xf) + 0x30;
		data=data>>4;
		if(temp>0x39)temp+=7;

		destaddr[7-i]=temp;
	}
	/*
	int i=0;
	int signal=0;
	for(i=0;i<16;i++)
	{
		ch=( argtable[0] >> (60-4*i) ) & 0xf;
		if(ch != 0) signal++;
		else if(signal) signal++;

		if(signal !=0)
		{
			ch+=0x30;
			if(ch>0x39) ch+=0x7;
			journal[y*64+x+signal-1]=ch;
		}
	}
	*/

}




void say(char* p,...)
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
		//是0，字符串结束了
		if(p[in] == '\0')break;

		//%d,%c,%lf,%llx.....
		else if(p[in]=='%')
		{
			if(p[in+1]=='x')
			{
				hexadecimal(destaddr+out,argtable[argcount]);
				argcount++;
				in+=2;
				out+=8;
			}
		}

		//normal
		else
		{
			destaddr[out]=p[in];
			in++;
			out++;
		}

	}//while(1)finish

	*(QWORD*)(journalhome+journalsize-8)+=0x40;
}

#define QWORD unsigned long long
#define DWORD unsigned int

#define consolehome 0xc00000
#define consolesize 0x100000

#define journalhome 0xd00000
#define journalsize 0x100000




int hexadecimal(char* dest,QWORD data)
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
		*dest=0x30;
		return 1;
	}

	//显示数字
	for(i=0;i<count;i++)
	{
		temp=(data&0xf) + 0x30;
		if(temp>0x39)temp+=7;

		data=data>>4;
		dest[count-1-i]=temp;
	}

	return count;
}




int decimal(char* dest,QWORD data)
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
		*dest=0x30;
		return 1;
	}

	//显示数字
	for(i=0;i<count;i++)
	{
		temp=(data%10) + 0x30;
		if(temp>0x39)temp+=7;

		data=data/10;
		dest[count-1-i]=temp;
	}

	return count;
}




//		~=sprintf();
void arg2string(QWORD* argtable,char* dest)
{
	//保存传入的参数
	char* source=(char*)(argtable[0]);			//0号是source字符串位置
	int argcount=1;			//从1号开始

	//把传入的字符串写进buffer里面
	int in=0;
	int out=0;
	while(1)
	{
		if(out>=0x40)break;
		if(source[in] == '\0')break;		//是0，字符串结束了

		else if(source[in]==0x9)
		{
			*(DWORD*)(dest+out)=0x20202020;
			in++;
			out+=4;
		}
		else if(source[in]=='%')		//%d,%c,%lf,%llx.....
		{
			if(source[in+1]=='x')
			{
				in+=2;
				out+=hexadecimal(dest+out,argtable[argcount]);
				argcount++;
			}
			else if(source[in+1]=='d')
			{
				in+=2;
				out+=decimal(dest+out,argtable[argcount]);
				argcount++;
			}
			else
			{
				dest[out]=source[in];
				in++;
				out++;
			}
		}
		else				//normal
		{
			dest[out]=source[in];
			in++;
			out++;
		}

	}//while(1)finish
}




void say(QWORD arg0,QWORD arg1,QWORD arg2,QWORD arg3,QWORD arg4,QWORD arg5)
{
	//保存传进来的6个参数
	QWORD argtable[6]={arg0,arg1,arg2,arg3,arg4,arg5};

	//这次往哪儿写（要确保不写到指定以外的地方）
	unsigned long long temp=*(QWORD*)(consolehome+consolesize-8);
	if(temp>=consolesize-0x80)
	{
		temp=0;
		*(QWORD*)(consolehome+consolesize-8)=0;
	}
	char* dest=(char*)(consolehome+temp);

	//把传入的字符串写进buffer里面
	*(QWORD*)(dest)=0x4645444342413938;				//time
	*(QWORD*)(dest+8)=0x3736353433323130;
	*(QWORD*)(dest+0x10)=0x7473616d20202020;				//name
	*(QWORD*)(dest+0x18)=0x2020202020207265;

	arg2string(argtable,dest+32);

	//这里有点特殊,要自己更新屏幕,还是让后面的函数自己整屏得刷新
	//updatescreen(leftx,rightx,upy,downy);

	//下一次怎办
	*(QWORD*)(consolehome+consolesize-8)+=0x80;
}
void diary(QWORD arg0,QWORD arg1,QWORD arg2,QWORD arg3,QWORD arg4,QWORD arg5)
{
	//保存传进来的6个参数
	QWORD argtable[6]={arg0,arg1,arg2,arg3,arg4,arg5};

	//这次往哪儿写（要确保不写到指定以外的地方）
	unsigned long long temp=*(QWORD*)(journalhome+journalsize-8);
	if(temp>=journalsize-0x40)
	{
		temp=0;
		*(QWORD*)(journalhome+journalsize-8)=0;
	}
	char* dest=(char*)(journalhome+temp);

	//往里写
	arg2string(argtable,dest);

	//下一次怎办
	*(QWORD*)(journalhome+journalsize-8)+=0x40;
}













/*
	int in=0;
	int out=32;
	while(1)
	{
		if(p[in] == '\0')break;		//是0，字符串结束了

		else if(p[in]==0x9)
		{
			*(DWORD*)(dest+out)=0x20202020;
			in++;
			out+=4;
		}
		else if(p[in]=='%')		//%d,%c,%lf,%llx.....
		{
			if(p[in+1]=='x')
			{
				in+=2;
				out+=hexadecimal(dest+out,argtable[argcount]);
				argcount++;
			}
			else if(p[in+1]=='d')
			{
				in+=2;
				out+=decimal(dest+out,argtable[argcount]);
				argcount++;
			}
			else
			{
				dest[out]=p[in];
				in++;
				out++;
			}
		}
		else				//normal
		{
			dest[out]=p[in];
			in++;
			out++;
		}

	}//while(1)finish
	//保存传入的寄存器值
	//register unsigned long long rsi asm("rsi");
	//register unsigned long long rdx asm("rdx");
	//register unsigned long long rcx asm("rcx");
	//register unsigned long long r8 asm("r8");
	//register unsigned long long r9 asm("r9");
	//unsigned long long argtable[5]={rsi,rdx,rcx,r8,r9};
	//int argcount=0;

*/
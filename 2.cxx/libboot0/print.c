#define u64 unsigned long long
#define u32 unsigned int
#define onemega 0x100000
static char* inputqueue;        //stdin
static char* outputqueue;       //stdout
static char* journalqueue;      //stderr




int hexadecimal(char* dest, u64 data)
{
	u64 temp;
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




int decimal(char* dest, u64 data)
{
	u64 temp;
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
void arg2string(u64* argtable, char* dest)
{
	//保存传入的参数
	char* source=(char*)(argtable[0]);			//0号是source字符串位置
	int argcount=1;			//从1号开始

	//把传入的字符串写进buffer里面
	int in=0;
	int out=0;
	while(1)
	{
		if(out>=0x80)break;
		if(source[in] == '\0')break;		//是0，字符串结束了

		else if(source[in]==0x9)
		{
			*(u32*)(dest+out)=0x20202020;
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




void say(u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	//保存传进来的6个参数
	u64 argtable[6]={arg0,arg1,arg2,arg3,arg4,arg5};

	//这次往哪儿写（要确保不写到指定以外的地方）
	u64 temp=*(u64*)(outputqueue+onemega-8);
	if(temp >= onemega-0x80)
	{
		temp=0;
		*(u64*)(outputqueue+onemega-8)=0;
	}
	char* dest=(char*)(outputqueue+temp);

	//把传入的字符串写进buffer里面
	*(u64*)(dest)=0x4645444342413938;	//time
	*(u64*)(dest+8)=0x3736353433323130;
	*(u64*)(dest+0x10)=0x7473616d20202020;	//name
	*(u64*)(dest+0x18)=0x2020202020207265;

	arg2string(argtable,dest+32);

	//这里有点特殊,要自己更新屏幕,还是让后面的函数自己整屏得刷新
	//updatescreen(leftx,rightx,upy,downy);

	//下一次怎办
	*(u64*)(outputqueue+onemega-8)+=0x80;
}
void diary(u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	//保存传进来的6个参数
	u64 argtable[6]={arg0,arg1,arg2,arg3,arg4,arg5};

	//这次往哪儿写（要确保不写到指定以外的地方）
	u64 temp=*(u64*)(journalqueue+onemega-8);
	if(temp >= onemega-0x80)
	{
		temp=0;
		*(u64*)(journalqueue+onemega-8)=0;
	}
	char* dest=(char*)(journalqueue+temp);

	//往里写
	arg2string(argtable,dest);

	//下一次怎办
	*(u64*)(journalqueue+onemega-8)+=0x80;
}




void initprint(void* addr)
{
	inputqueue = addr;
	outputqueue = addr+0x100000;
	journalqueue = addr+0x200000;
}

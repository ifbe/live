#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define onemega 0x100000
int writeuart(void*, int);




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




int fmt(char* dest, u64* argtable)
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
	return out;
}
void say(u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
	//保存传进来的6个参数
	int ret;
	char* dest;
	u64 argtable[6]={arg0,arg1,arg2,arg3,arg4,arg5};

	//这次往哪儿写（要确保不写到指定以外的地方）
	ret = *(u64*)(journalqueue+onemega-8);
	if(ret >= onemega-0x80)
	{
		ret = 0;
		*(u64*)(journalqueue+onemega-8)=0;
	}
	dest = (char*)(journalqueue + ret);
	*(u64*)(journalqueue+onemega-8)+=0x80;

	//往里写
	ret = fmt(dest, argtable);
	ret = writeuart(dest, ret);
}




void printmemory(u8* buf, int len)
{
	u64 temp=*(u64*)(journalqueue+onemega-8);
	if(temp >= onemega-0x80)
	{
		temp = 0;
		*(u64*)(journalqueue+onemega-8) = 0;
	}
	u8* dst = (void*)(journalqueue+temp);

	u8 c;
	int j,k = 0;
	for(j=0;j<len;j++)
	{
		c = buf[j]>>4;
		c += 0x30;
		if(c > 0x39)c += 0x27;
		dst[k+0] = c;

		c = buf[j]&0xf;
		c += 0x30;
		if(c > 0x39)c += 0x27;
		dst[k+1] = c;

		c = buf[j];
		if(c<0x20|c>0x7e)c=0x20;
		dst[k+0x40] = 0x20;
		dst[k+0x41] = c;
		k += 2;

		if((j>0)&&((j&0x1f) == 0x1f))k += 0x40;
	}

	*(u64*)(journalqueue+onemega-8) += k&0xffffff80;
}




void initprint(void* addr)
{
	inputqueue = addr;
	outputqueue = addr+0x100000;
	journalqueue = addr+0x200000;
}

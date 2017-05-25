#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long


void blank2zero(u64* name)
{
	int i;
	u64 temp;
	for(i=0;i<8;i++)
	{
		temp = ( (*name)>>(i*8) )&0xff;
		if(temp == 0x20)
		{
			*name -= ((u64)0x20)<<(i*8);
		}
	}
}


void zero2blank(u64* name)
{
	int i;
	u64 temp;
	for(i=0;i<8;i++)
	{
		temp = ( (*name)>>(i*8) )&0xff;
		if(temp == 0)
		{
			*name += ((u64)0x20)<<(i*8);
		}
	}
}


void small2capital(u64* name)
{
	int i;
	u64 temp;
	for(i=0;i<8;i++)
	{
		temp = ( (*name) >> (i*8) )&0xff;
		if(temp>='a' && temp<='z')
		{
			*name -= ( (u64)0x20 )<<(i*8);
		}
	}
}


void string2data(u8* str,u64* data)
{
	int i;
	*data=0;
	for(i=0;i<8;i++)
	{
		if(str[i]<=0x20) break;
		*data+=((u64)str[i])<<(i*8);
	}
}



int data2decstr(u64 data,u8* string)
{
	int j,max;
	u64 temp;

	max = 0;
	temp = data;
	for(j=0;j<0x10;j++)
	{
		max++;
		temp /= 10;
		if(temp == 0)break;
	}

	temp = data;
	for(j=1;j<=max;j++)
	{
		string[max-j] = (temp%10)+0x30;
		temp/=10;
	}

	return max;
}



int data2hexstr(u64 data,u8* str)
{
	int j,max;
	u64 temp;

	max = 0;
	temp = data;
	for(j=0;j<0x10;j++)
	{
		max++;
		temp >>= 4;
		if(temp == 0)break;
	}

	for(j=1;j<=max;j++)
	{
		temp = data&0xf;
		data >>= 4;

		temp+=0x30;
		if(temp>=0x3a)temp+=7;
		str[max-j] = temp;
	}
	return max;
}



//double类型转换成10进制字符串
int double2decstr(double data, u8* str)
{
	double temp;
	int offset;
	int count;

	//符号部分
	offset=0;
	if(data<0)
	{
		str[0]='-';
		offset+=1;

		data=-data;
	}

	//整数部分
	offset+=data2decstr( (u64)data , str+offset );

	//小数点
	str[offset]='.';
	offset++;

	//小数部分
	temp=(double)(u64)data;
	temp=data-temp;

	if(temp<0.0000000000000000000000000000000001)
	{
		//特别小
		str[offset]=0x30;
		offset++;
	}
	else
	{
		//一般小数
		while(temp<0.1)
		{
			temp*=10;
			str[offset]=0x30;
			offset++;
		}
		temp=temp*10000000;
		count=data2decstr( (u64)temp , str+offset );
		offset+=count;
	}

	//0
	str[offset]=0;
	return offset;
}
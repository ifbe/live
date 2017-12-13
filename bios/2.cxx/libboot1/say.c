#include<stdarg.h>
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define onemega 0x100000
int data2decstr(u64 data, void* str);
int data2hexstr(u64 data, void* str);
int double2decstr(double data, void* str);
void writeuart(void*, int);




static void* inputqueue;        //stdin
	static u64* incur;			//real position
	static u64* inwin;			//insert position
static void* outputqueue;       //stdout
	static u64* outcur;			//real position
	static u64* outwin;			//display position
static void* journalqueue;      //stderr
	static u64* logcur;			//real position
	static u64* logwin;			//display position
void initprint(char* buf)
{
	inputqueue = buf;
	incur = inputqueue+0x100000-16;
	inwin = inputqueue+0x100000-8;

	outputqueue = buf+0x100000;
	outcur = outputqueue+0x100000-16;
	outwin = outputqueue+0x100000-8;

	journalqueue = buf+0x200000;
	logcur = journalqueue+0x100000-16;
	logwin = journalqueue+0x100000-8;
}




u64 fixdata(u64 data, int val)
{
	u64 mask;
	if(val == 0)return data;

	mask = 0;
	while(val > 0)
	{
		mask = (mask<<8) | 0xff;
		val--;
	}
	return data&mask;
}
int convert_c(char* buf, int align, int k, int ch)
{
	if(ch == '\n')
	{
		buf[k] = '\n';
		k += 0x80-((k+align)%0x80);
	}
	else if(ch == '	')
	{
		do
		{
			buf[k] = 0x20;
			k++;
		}while( ( (align+k) % 8 ) != 0);
	}
	else
	{
		buf[k] = ch;
		k++;
	}
	return k;
}
int convert(char* buf, int len, char* str, int align, va_list arg)
{
	int j,k,t;
	int num,val,flag;
	u64 _x;
    char* _s;
    double _f;

	j = k = 0;
	while(1)
	{
		if(k >= len)return len;
		if(str[j] == 0)return k;

		if(str[j] == '%')
		{
			val = 0;
			num = j+1;
			flag = str[num];
			if(flag == '-')num++;
			while( (str[num] >= 0x30) && (str[num] <= 0x39) )
			{
				val = (val*10) + (str[num]-0x30);
				num++;
			}

			if(str[num] == 'c')
			{
				_x = va_arg(arg, int);
				k = convert_c(buf, align, k, _x);

				j = num+1;
				continue;
			}
			else if(str[num] == 'd')
			{
				_x = va_arg(arg, u64);
				k += data2decstr(_x, buf+k);

				j = num+1;
				continue;
			}
			else if(str[num] == 'f')
			{
				_f = va_arg(arg, double);
				k += double2decstr(_f, buf+k);

				j = num+1;
				continue;
			}
			else if(str[num] == 'x')
			{
				_x = va_arg(arg, u64);
				_x = fixdata(_x, val);

				if( (flag == '0')&&(val == 2) )
				{
					if(_x < 0x10)
					{
						buf[k] = '0';
						k += 1;
					}
				}

				t = k;
				k += data2hexstr(_x, buf+k);
				if( (flag == '-')&&(val > 0) )
				{
					for(;k<t+val;k++)
					{
						buf[k] = ' ';
					}
				}

				j = num+1;
				continue;
			}
			else if(str[num] == 's')
			{
				_s = va_arg(arg, char*);
				while(1)
				{
					if(k >= len)return len;

					k = convert_c(buf, align, k, *_s);

					_s++;
					if(*_s == 0)break;
				}
				j = num+1;
				continue;
			}
		}

		k = convert_c(buf, align, k, str[j]);
		j++;
	}
	return k;
}
void printout(int cur, int len)
{
	int j,k;
	u8* p;

	p = outputqueue;
	j = cur;
	while(1)
	{	
		if(j >= cur+len)
		{
			if(j%0x80 == 0)break;

			k = j - (j%0x80);
			if(k<cur)k=cur;

			writeuart(p+k, j-k);
			break;
		}
		else if(p[j] == '\n')
		{
			k = j - (j%0x80);
			if(k<cur)k=cur;

			writeuart(p+k, j-k+1);
			j = (j+0x80) - (j%0x80);
		}
		else
		{
			if(j%0x80 == 0x7f)
			{
				k = j - (j%0x80);
				if(k<cur)k=cur;

				writeuart(p+k, j-k+1);
			}
			j++;
		}
	}
}




int fmt(char* buf, int len, char* str, ...)
{
	int ret;
	va_list arg;
	va_start(arg, str);
	ret = convert(buf, len, str, 0, arg);
	va_end(arg);
    return ret;
}
void say(char* str, ...)
{
	int cur,ret;
	va_list arg;
	va_start(arg, str);

	//read position
	cur = *outcur;

	//snprintf
	ret = convert(outputqueue+cur, 0x1000, str, cur%0x80, arg);

	//
	va_end(arg);

	//debugport
	printout(cur, ret);

	//write position
	cur = cur+ret;
	if(cur > 0xf0000)cur = 0;
	*outcur = cur;

}



void printmemory(u8* addr,int size)
{
	int x,y;
	u8 ch;
	u8* p;
	say("\n");

	for(y=0;y<size/16;y++)
	{
		p = addr+y*16;

		//addr
		if((y%16) == 0)say( "@%-13x" , (u64)p );
		else say("+%-13x",y*16);

		//hex
		for(x=0;x<=0xf;x++)
		{
			say("%02x ",p[x]);
		}

		//ascii
		for(x=0;x<=0xf;x++)
		{
			ch = p[x];
			if( (ch>=0x7f)|(ch<0x20) )ch=0x20;
			say("%c",ch);
		}
		say("\n");
	}
	if(y*16 >= size)return;

	p = addr+y*16;
	say("+%-13x",y*16);
	for(x=0;x<16;x++)
	{
		if(x >= size%16)say("   ");
		else say("%02x ",p[x]);
	}
	for(x=0;x<16;x++)
	{
		if(x >= size%16)say(" ");
		else
		{
			ch = p[x];
			if( (ch>=0x7f)|(ch<0x20) )ch=0x20;
			say("%c",ch);
		}
	}
	say("\n");
}

/*
void printmemory(u8* buf, int len)
{
	u64 temp=*(u64*)(journalqueue+onemega-16);
	if(temp >= onemega-0x80)
	{
		temp = 0;
		*(u64*)(journalqueue+onemega-16) = 0;
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

	*(u64*)(journalqueue+onemega-16) += k&0xffffff80;
}
*/
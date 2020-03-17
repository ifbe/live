#include<stdarg.h>
#define s8 char
#define s16 short
#define s32 int
#define s64 long long
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define onemega 0x100000
void stdio_write(void*, int);
int myvsnprintf(u8* buf, int len, u8* fmt, __builtin_va_list arg);
int mysnprintf(u8* buf, int len, u8* fmt, ...);




static void* outbuf;       //stdout
static u64* outcur;			//real position
void initprint(char* buf)
{
	outbuf = buf+0x100000;
	outcur = outbuf+0x100000-16;
}




void printout(int cur, int len)
{
	int j,k;
	u8* p;

	p = outbuf;
	j = cur;
	while(1)
	{	
		if(j >= cur+len)
		{
			if(j%0x80 == 0)break;

			k = j - (j%0x80);
			if(k<cur)k=cur;

			stdio_write(p+k, j-k);
			break;
		}
		else if(p[j] == '\n')
		{
			k = j - (j%0x80);
			if(k<cur)k=cur;

			stdio_write(p+k, j-k+1);
			j = (j+0x80) - (j%0x80);
		}
		else
		{
			if(j%0x80 == 0x7f)
			{
				k = j - (j%0x80);
				if(k<cur)k=cur;

				stdio_write(p+k, j-k+1);
			}
			j++;
		}
	}
}
void say(u8* str, ...)
{
	int cur,ret;
	va_list arg;
	va_start(arg, str);

	//read position
	cur = *outcur;

	//snprintf
	//ret = myvsnprintf(outbuf+cur, 0x1000, str, cur%0x80, arg);
	ret = myvsnprintf(outbuf+cur, 0x1000, str, arg);

	//
	va_end(arg);

	//debugport
	printout(cur, ret);

	//write position
	cur = cur+ret;
	if(cur > 0xf0000)cur = 0;
	*outcur = cur;

}




void printmemory(u8* buf, int len)
{
	u8 c;
	int j,k;
	u8 tmp[128];

	while(1)
	{
		if(len <= 0)break;
		if(len <= 16)k = len;
		else k = 16;

		j = mysnprintf(tmp, 80, (void*)"@%-13llx", buf);
		for(;j<80;j++)tmp[j]=0x20;

		for(j=0;j<k;j++)
		{
			c = ((buf[j]>>4)&0xf)+0x30;
			if(c > 0x39)c += 7;
			tmp[(3*j) + 14] = c;

			c = (buf[j]&0xf)+0x30;
			if(c > 0x39)c += 7;
			tmp[(3*j) + 15] = c;

			c = buf[j];
			if((c<0x20)|(c>=0x7f))c = 0x20;
			tmp[14+48+j] = c;
		}
		say((void*)"%.*s\n", 14+48+16, tmp);

		buf += 16;
		len -= 16;
	}
}

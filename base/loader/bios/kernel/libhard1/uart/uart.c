#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int readuart(void*, int);
struct event
{
	u64 why;
	u64 what;
	u64 where;
	u64 when;
};
int waituart(struct event* ev)
{
	int ret;
	u8 buf[1];
	ret = readuart(buf, 1);
	if(ret == 0)return 0;
	if(0x7f == buf[0])buf[0] = 0x8;

	ev->why = buf[0];
	ev->what = 0x72616863;
	return 1;
}

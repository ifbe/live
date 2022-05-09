#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int writeuart(void*,int);
//
int waitkbd(void*);
int waituart(void*);
//
void term_readbycli();
void term_write(u64 why, u64 what);




struct event
{
        u64 why;
        u64 what;
        u64 where;
        u64 when;
};
void stdio_read()
{
	struct event ev[1];
	term_readbycli();

	while(waitkbd(ev) > 0)term_write(ev->why, ev->what);
	while(waituart(ev)> 0)term_write(ev->why, ev->what);
}
void stdio_write(void* buf, int len)
{
	writeuart(buf,len);
}

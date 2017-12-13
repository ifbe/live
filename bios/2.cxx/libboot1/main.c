#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




//libui1
void characterread();
void characterwrite(u64 key, u64 type);
//libui0
void writescreen();
//
void* waitevent();




struct event
{
	u64 why;
	u64 what;
	u64 where;
	u64 when;
};
void main()
{
	struct event* ev;
	while(1)
	{
		characterread();
		writescreen();

repeat:
		ev = waitevent();
		if(ev == 0)goto repeat;

		characterwrite(ev->why, ev->what);
	}
}

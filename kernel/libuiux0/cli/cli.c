#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int waitkbd(void*);
int writekbd(void*, int);
//
int waituart(void*);
int writeuart(void*, int);
//
void termread_cli();
void termwrite(u64 why, u64 what);




struct event
{
        u64 why;
        u64 what;
        u64 where;
        u64 when;
};
void readcli()
{
	struct event ev[1];
	termread_cli();

	while(waitkbd(ev) > 0){
		//writekbd(ev,1);
		writeuart(ev,1);
		termwrite(ev->why, ev->what);
	}
	while(waituart(ev)> 0){
		//writekbd(ev,1);
		writeuart(ev,1);
		termwrite(ev->why, ev->what);
	}
}

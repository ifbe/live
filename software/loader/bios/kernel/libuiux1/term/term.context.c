#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
int command(u8*);
int stdio_write(void*, int);
//
void printstring(int x, int y, int size, u8* ch, u32 fgcolor, u32 bgcolor);
void printtext(void* p, int size, int x0, int y0, int dx, int dy, u32 fgc, u32 bgc);
//
void printmemory(void*, int);
void say(char*,...);




//
static u32* window;
//
static u8* input;
static u32 count=0;
//
static u8* output;
static u32 offset=0;




void terminit()
{
	input = (void*)0x1000000;
	count = 0;

	output = (void*)0x1100000;
	offset = 0;

	window = (void*)0x2000000;
}
void termcreate()
{
	say("disk\n");
	say("diskread 200\n");
	say("part\n");
	say("partread\n");
	say("file\n");
	say("fileread 42.bin\n");
	say("arp\n");
	say("icmp\n");
	say("udp 1111\n");
	say("reboot\n");
	say("poweroff\n");
	say("print\n");
	say("jump\n");
	say("live>>>>");
}




void termread_image()
{
	int x,y;
	for(y=0;y<768;y++){
		for(x=0;x<1024;x++)window[(y<<10)+x] = 0;
	}

	y = 0;
	x = *(int*)(output + 0xffff0);
	for(;x>=1;x--){
		if('\n' == output[x])y++;
		if(y >= 47)break;
	}
	printtext(output+x, 1, 0, 0, 1024, 768, 0xffffff, 0xff000000);

	//printstring(0, 752, 1, input, 0xffffff, 0xff000000);
}
void termread_a0000()
{
	int x,y;
	u8 color;
	u8* p = (void*)window;

	for(y=0;y<200;y++)
	{
		for(x=0;x<320;x++)
		{
			color = ((y*16/200)<<4) + (x*16/320);
			p[320*y + x] = color;
		}
	}
}
void termread_b8000()
{
	u8* p;
	u8* q;
	int x,y;
	int c,t;
	u32 temp = *(u32*)(output + 0xffff0);
	if(temp < offset)temp = 0;
	else
	{
		temp -= offset;
		if(temp < 0x80*24)temp = 0;
		else temp = temp-0x80*24;
	}

	p = (void*)window;
	q = output+temp;
	for(y=0;y<25;y++)
	{
		for(x=0;x<80;x++)
		{
			c = q[y*0x80 + x];
			if(c < 0x20)c = 0x20;

			t = (y*80+x)*2;
			p[t+0] = c;
			p[t+1] = 0x70;
		}
	}

	for(x=0;x<80;x++)
	{
		p[2*(80*24+x) + 0] = input[x];
		p[2*(80*24+x) + 1] = 0x70;
	}
}
void term_readbywnd()
{
	u32 type = *(u32*)0x2008;
	if(type == 0xb8000)termread_b8000();
	else if(type == 0xa0000)termread_a0000();
	else termread_image();
}
void term_readbycli()
{
}



#define _kbd_ 0x64626b
#define _char_ 0x72616863
void term_write(u64 key, u64 type)
{
	//say("%llx,%llx\n",type,key);
	if(type == _kbd_)
	{
		if(key == 0x1b)		//esc
		{
			offset = 0;
		}
		else if(key == 0xf1){
			say("f1\n");
		}
		else if(key == 0x47)	//home
		{
			offset = 0x100000;
		}
		else if(key == 0x4f)	//end
		{
			offset = 0;
		}
		else if(key == 0x49)	//page up
		{
			offset += 0x800;
		}
		else if(key == 0x51)	//page down
		{
			if(offset>0x800)offset -= 0x800;
		}

		return;
	}
	if(type == _char_)
	{
		if(key == 0x8)
		{
			if(count>0){
				int* realoffs = (int*)(output + 0xffff0);
				if(*realoffs > 0)*realoffs -= 1;
				output[*realoffs] = 0;

				stdio_write("\b", 1);
				count--;
			}
			input[count] = 0;
		}
		else if((key == 0xd)|(key == 0xa))
		{
			say("\n");
			command(input);
			say("live>>>>");

			for(count=0;count<128;count++)input[count] = 0;
			count = 0;
		}
		else
		{
			if(count > 127)return;
			say("%c", key);

			input[count] = (u8)key;
			count ++;
		}
	}
}

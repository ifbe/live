#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
#define screeninfo 0x2000




void start()
{
	int x,y;
	u8* tmp;
	u8* buf = (u8*)(u64)(*(u32*)screeninfo);
	u32 fmt = *(u32*)(screeninfo+8);

	while(1){
		tmp = buf;
		for(y=0;y<767;y++){
			for(x=0;x<1024;x++){
				tmp[0] = x/3;
				tmp[1] = y/3;
				tmp[2] = 0x80;
				tmp[3] = 0;

				tmp += fmt;
			}
		}
	}
}

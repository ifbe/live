#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int ahci_readblock(u64 dev, u64 off, u64 buf, u64 len);
int ahci_list();
int sd_readblock(u64 dev, u64 off, u64 buf, u64 len);
int sd_list();
//
void say(char*,...);




static int chosen = 0;
void disk_list()
{
	int ret;

	say("ahci:\n");
	ret = ahci_list();
	if(ret)chosen = 'a';

	say("sd:\n");
	ret = sd_list();
	if(ret)chosen = 's';
}
void disk_focus()
{
}


//read the chosen device, byte to sector
int diskread(u64 fd, u64 off, u64 buf, u64 len)
{
	u64 j = 0;
	u64 sec = off >> 9;
	u64 cnt = (len+0x1ff)>>9;
	while(j+0x80 < cnt){
		if('a'==chosen)ahci_readblock(0, sec+j, buf+j*0x200, 0x80);
		if('s'==chosen)sd_readblock(0, sec+j, buf+j*0x200, 0x80);
		j += 0x80;
	}
	if('a'==chosen)ahci_readblock(0, sec+j, buf+j*0x200, cnt-j);
	if('s'==chosen)sd_readblock(0, sec+j, buf+j*0x200, cnt-j);
	return len;
}

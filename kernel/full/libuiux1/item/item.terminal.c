#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
#define powerport (*(u16*)0xffc)
#define powerdata ((*(u16*)0xffe)|0x2000)
//
int diskread(u64 fd, u64 off, void* buf, int len);
void ahci_list();
void ahci_choose();
void identify();
//
void pt_check(void*);
int pt_read(u64 fd,u64 off, void* buf, int len);
//
void fs_check(void*);
int fs_read(u64 fd,u64 off, void* buf, int len);
//
void out8(u32, u8);
void out16(u32, u16);
//
int hexstr2data(void*, void*);
int ncmp(void*, void*, int);
int cmp(void*, void*);
//
void printmemory(void*, int);
void say(char*,...);




void command(u8* input)
{
	say("%s\n",input);
	if(0 == input[0])return;
	else if(0 == ncmp(input,"reboot",6)){
		out8(0x64,0xfe);
	}
	else if(0 == ncmp(input,"poweroff",8)){
		u16 port = powerport;
		u16 data = powerdata;
		say("port=%x,data=%x\n",port,data);
		out16(port, data);
	}
	else if(0 == ncmp(input,"print",5)){
		u64 data;
		hexstr2data(input+6, &data);
		printmemory((void*)data,0x200);
	}
	else if(0 == ncmp(input,"ahci.cd",7)){
		ahci_choose();
	}
	else if(0 == ncmp(input,"ahci.id",7)){
		identify();
	}
	else if(0 == ncmp(input,"disk.read",9)){
		u64 data;
		hexstr2data(input+10, &data);
		diskread(0, data, (void*)0x100000, 0x100000);
	}
	else if(0 == ncmp(input,"disk",4)){
		ahci_list();
	}
	else if(0 == ncmp(input,"pt.read",7)){
		pt_read(0, 0, (void*)0x100000, 0x100000);
	}
	else if(0 == ncmp(input,"pt",2)){
		pt_check((void*)0x100000);
	}
	else if(0 == ncmp(input,"fs.read",7)){
		fs_read((u64)(input+8), 0, (void*)0x100000, 0x100000);
	}
	else if(0 == ncmp(input,"fs",2)){
		fs_check((void*)0x100000);
	}
	else if(0 == ncmp(input,"bss",3)){
		int j;
		u8* tmp = (void*)0x100000;
		for(j=0;j<0x700000;j++)tmp[j] = 0;
	}
	else if(0 == ncmp(input,"42",2)){
		int (*fun)() = (void*)0x100000;
		fun();
	}
}

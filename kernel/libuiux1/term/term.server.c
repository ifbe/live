#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
#define powerport (*(u16*)0xffc)
#define powerdata ((*(u16*)0xffe)|0x2000)
//
int disk_list(void*);
int diskread(u64 fd, u64 off, void* buf, int len);
//
int pt_list(void*);
int pt_read(u64 fd,u64 off, void* buf, int len);
//
int fs_list(void*);
int fs_read(u64 fd,u64 off, void* buf, int len);
//
void reboot();
void poweroff();
void arprequest(u8*);
void icmprequest(u8*);
//
int hexstr2data(void*, void*);
int ncmp(void*, void*, int);
int cmp(void*, void*);
//
void printmemory(void*, int);
void say(char*,...);




void command(u8* input)
{
	//say("%s\n",input);
	if(0 == input[0])return;

//-------------------disk-----------------
	else if(0 == ncmp(input,"diskread",8)){
		u64 offs;
		u8* addr = (void*)0x100000;
		hexstr2data(input+9, &offs);
		diskread(0, offs, addr, 0x1000);
		printmemory(addr, 0x200);
	}
	else if(0 == ncmp(input,"disk",4)){
		disk_list((void*)0x100000);
	}

//------------------part----------------
	else if(0 == ncmp(input,"partread",8)){
		u8* addr = (void*)0x100000;
		pt_read(0, 0, addr, 0x1000);
		printmemory(addr, 0x200);
	}
	else if(0 == ncmp(input,"part",4)){
		pt_list((void*)0x100000);
	}

//------------------file----------------
	else if(0 == ncmp(input,"fileread",8)){
		int j;
		u8* addr = (void*)0x100000;
		for(j=0;j<0x700000;j++)addr[j] = 0;

		fs_read((u64)(input+9), 0, addr, 0x100000);
		printmemory(addr, 0x200);
	}
	else if(0 == ncmp(input,"file",4)){
		fs_list((void*)0x100000);
	}

//------------------net-----------------
	else if(0 == ncmp(input,"arp",3)){
		arprequest(0);
	}
	else if(0 == ncmp(input,"icmp",4)){
		icmprequest(0);
	}

//------------------bye-----------------
	else if(0 == ncmp(input,"reboot",6)){
		reboot();
	}
	else if(0 == ncmp(input,"poweroff",8)){
		poweroff();
	}
	else if(0 == ncmp(input,"print",5)){
		u64 data;
		hexstr2data(input+6, &data);
		printmemory((void*)data,0x200);
	}
	else if(0 == ncmp(input,"42",2)){
		int (*fun)() = (void*)0x100000;
		fun();
	}
}

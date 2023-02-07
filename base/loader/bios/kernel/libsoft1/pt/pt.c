#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define hex16(a,b) (a | (b<<8))
#define hex32(a,b,c,d) (a | (b<<8) | (c<<16) | (d<<24))
#define hex64(a,b,c,d,e,f,g,h) (hex32(a,b,c,d) | (((u64)hex32(e,f,g,h))<<32))
#define _EFI_PART_ hex64('E','F','I',' ','P','A','R','T')
int diskread(u64 fd, u64 off, u8* buf, u64 len);
int ncmp(void*,void*,int);
void printmemory(void* , ...);
void say(void* , ...);




static u64 chosen = 0;





//[+0x400,+0x4400],每个0x80,总共0x80个
struct gptpart{
	u8 guid_type[16];	//[+0,+0xf]:类型guid
	u8 guid_part[16];	//[+0x10,+0x1f]:分区guid
	u64 lba_start;		//[+0x20,+0x27]:起始lba
	u64 lba_end;		//[+0x28,+0x2f]:末尾lba
	u64 flag;			//[+0x30,+0x37]:属性标签
	u8 name[0x48];		//[+0x38,+0x7f]:名字
};
static u8 guid_bbp[16] = {0x48,0x61,0x68,0x21,0x49,0x64,0x6F,0x6E,0x74,0x4E,0x65,0x65,0x64,0x45,0x46,0x49};
static u8 guid_efi[16] = {0x45,0x46,0x49,0x20,0x50,0x41,0x52,0x54,0x00,0x00,0x01,0x00,0x5C,0x00,0x00,0x00};
static u8 guid_fat[16] = {0x28,0x73,0x2a,0xC1,0x1F,0xF8,0xD2,0x11,0xBA,0x4B,0x00,0xA0,0xC9,0x3E,0xC9,0x3B};
static u8 guid_hfs[16] = {0x00,0x53,0x46,0x48,0x00,0x00,0xaa,0x11,0xaa,0x11,0x00,0x30,0x65,0x43,0xec,0xac};
static u8 guid_apfs[16]= {0xEF,0x57,0x34,0x7C,0x00,0x00,0xAA,0x11,0xAA,0x11,0x00,0x30,0x65,0x43,0xEC,0xAC};
static u8 guid_ntfs[16]= {0xA2,0xA0,0xD0,0xEB,0xE5,0xB9,0x33,0x44,0x87,0xC0,0x68,0xB6,0xB7,0x26,0x99,0xC7};
void gpt_parse(u8* src)
{
	int j,k;
	u32 crc;
	struct gptpart* part;

	j = src[0x20c];
	if(j >= 0x14)j -= 0x14;
	//crc = crc32(0, src + 0x200, 0x10);
	//crc = crc32(crc, src + 0x214, 4);
	//crc = crc32(crc, src + 0x214, j);
	say("head crc:	%x, %x\n", *(u32*)(src+0x210), crc);
	//crc = crc32(0, src+0x400, 0x4000);
	say("body crc:	%x, %x\n", *(u32*)(src+0x258), crc);

	part = (void*)(src+0x400);
	for(j=0;j<0x80;j++)
	{
		if(0 == part[j].lba_start)continue;

		for(k=0;k<0x40;k++)part[j].name[k] = part[j].name[k*2];
		say("[%012x,%012x]:	%s\n", part[j].lba_start, part[j].lba_end, part[j].name);

		if(0)say("???\n");
		else if(0 == ncmp(part[j].guid_type, guid_bbp, 16))say("bbp!\n");
		else if(0 == ncmp(part[j].guid_type, guid_efi, 16))say("efi!\n");
		else if(0 == ncmp(part[j].guid_type, guid_fat, 16)){
			say("fat!\n");
			chosen = part[j].lba_start << 9;
		}
		else if(0 == ncmp(part[j].guid_type, guid_hfs, 16))say("hfs!\n");
		else if(0 == ncmp(part[j].guid_type, guid_apfs, 16))say("apfs!\n");
		else if(0 == ncmp(part[j].guid_type, guid_ntfs, 16))say("ntfs!\n");
		else printmemory(part[j].guid_type, 0x10);

		printmemory(part[j].guid_part, 0x10);
	}
}
int gpt_check(u8* addr)
{
        //[1fe, 1ff] = (0x55，0xaa)
	if(0x55 != addr[0x1fe])return 0;
	if(0xaa != addr[0x1ff])return 0;

        //[200, 207] = "EFI PART"
        u64 temp = *(u64*)(addr+0x200);
        if(temp != _EFI_PART_)return 0;

	return 1;
}




struct mbrpart{
        u8 bootflag;            //[+0]:活动标记
        u8 chs_start[3];        //[+0x1,+0x3]:开始磁头柱面扇区
        u8 parttype;            //[+0x4]:分区类型
        u8 chs_end[3];          //[+0x5,+0x7]:结束磁头柱面扇区
        u8 lba_start[4];        //[+0x8,+0xb]:起始lba
        u8 lba_count[4];        //[+0xc,+0xf]:大小
};
u32 hackforarmalign4(u8* p)
{
	return p[0] + (p[1]<<8) + (p[2]<<16) + (p[3]<<24);
}
void parse_mbr_one(struct mbrpart* part)
{
        u32 start,count;
        if(0 == part->parttype)return;

        start = hackforarmalign4(part->lba_start);
        count = hackforarmalign4(part->lba_count);
        say("[%08x,%08x]:\n", start, start+count-1);

	switch(part->parttype){
        case 0x05:
        case 0x15:
        {
                say("extend-chs\n");
                break;
        }
        case 0x0f:
        case 0x1f:
        {
                say("extend-lba\n");
                break;
        }
        case 0x04:
        case 0x06:
        case 0x0e:
        case 0x14:
        case 0x16:
        case 0x1e:
        {
                say("fat16\n");
                break;
        }
	case 0x0b:
        case 0x1b:
        {
		chosen = start << 9;
                say("fat32-chs\n");
                break;
        }
        case 0x0c:
        case 0x1c:
        {
		chosen = start << 9;
                say("fat32-lba\n");
                break;
        }
        case 0x07:
        case 0x17:
        {
                say("ntfs\n");
                break;
        }
        case 0x83:
        {
                say("ext\n");
                break;
        }
        default:{
                say("type=%02x\n",part->parttype);
        }
	}
}
void mbr_parse(u8* src)
{
        int j;
        src += 0x1be;
        for(j=0;j<0x40;j+=0x10){
		parse_mbr_one((void*)(src+j));
	}
	say("chosen=%x\n",chosen);
}
int mbr_check(u8* addr)
{
	if(0x55 != addr[0x1fe])return 0;
	if(0xaa != addr[0x1ff])return 0;
	return 1;
}




int pt_list(u8* addr)
{
	diskread(0, 0, (void*)addr, 0x1000);
	if(gpt_check(addr)){
		gpt_parse(addr);
		return 0;
	}
	if(mbr_check(addr)){
		mbr_parse(addr);
		return 0;
	}
	say("?\n");
	return 0;
}
void pt_focus()
{
}




int pt_read(u64 fd, u64 off, u8* buf, int len)
{
	return diskread(0, off+chosen, buf, len);
}

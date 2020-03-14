#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define hex16(a,b) (a | (b<<8))
#define hex32(a,b,c,d) (a | (b<<8) | (c<<16) | (d<<24))
#define hex64(a,b,c,d,e,f,g,h) (hex32(a,b,c,d) | (((u64)hex32(e,f,g,h))<<32))
#define _EFI_PART_ hex64('E','F','I',' ','P','A','R','T')
int diskread(u64 fd, u64 off, u8* buf, u64 len);
void say(char* , ...);




static u64 chosen = 0;




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
        u32 lba_start;          //[+0x8,+0xb]:起始lba
        u32 lba_count;          //[+0xc,+0xf]:大小
};
void parse_mbr_one(struct mbrpart* part)
{
        u32 start,count;
        if(0 == part->parttype)return;

        start = part->lba_start;
        count = part->lba_count;

        say("[%08x,%08x]:\n", start, start+count);
	chosen = start*0x200;

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
                say("fat32-chs\n");
                break;
        }
        case 0x0c:
        case 0x1c:
        {
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
        for(j=0;j<0x40;j+=0x10)parse_mbr_one((void*)(src+j));
}
int mbr_check(u8* addr)
{
	if(0x55 != addr[0x1fe])return 0;
	if(0xaa != addr[0x1ff])return 0;
	return 1;
}




int pt_check(u8* addr)
{
	diskread(0, 0, (void*)addr, 0x1000);
	if(gpt_check(addr)){
		say("gpt\n");
		return 0;
	}
	if(mbr_check(addr)){
		mbr_parse(addr);
		return 0;
	}
	say("?\n");
	return 0;
}
int pt_read(u64 fd, u64 off, u8* buf, int len)
{
	diskread(0, off+chosen, buf, len);
}

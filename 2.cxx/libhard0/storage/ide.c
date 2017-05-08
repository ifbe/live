#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define idehome 0x400000
u8 in8(u32 addr);
u16 in16(u32 addr);
u32 in32(u32 addr);
void out8(u32 port, u8 data);
void out16(u32 port, u16 data);
void out32(u32 port, u32 data);
void say(char* , ...);




void readide(u64 fd, u64 buf, u64 from, u64 count)
{
        u64 command=*(u64*)(idehome+0x10);	//command
        u64 control=*(u64*)(idehome+0x20);	//control
        char* dest=(char*)buf;
        volatile int i;
        volatile u32 temp;

        //channel reset
        out8(control+2,4);
        out16(0x80,0x1111);             //out8(0xeb,0)
        say("out %x %x\n",control+2,4);

        out8(control+2,0);
        say("out %x %x\n",control+2,0);

        out8(command+6,0xa0);
        say("out %x %x\n",command+6,0xa0);

        //50:hd exist
        i=0;
        while(1)
        {
                i++;
                if(i>0xfffff)
                {
                        say("device not ready:%x\n",temp);
                        return;
                }

                temp=in8(command+7);
                if( (temp&0x80) == 0x80 )continue;                      //busy
                if( (temp&0x40) == 0x40 )break;                 //Device ReaDY?
        }

        //count
        out8(command+2,count&0xff);
        say("total:%x\n",count);

        //lba
        out8(command+3,from&0xff);
        out8(command+4,(from>>8)&0xff);
        out8(command+5,(from>>16)&0xff);
        say("lba:%x\n",from);

        //mode
        out8(command+6,0xe0);
        say("out %x %x\n",command+6,0xe0);

        //start reading
        out8(command+7,0x20);
        say("out %x %x\n",command+7,0x20);

        //check hd data ready?
        i=0;
        while(1)
        {
                i++;
                if(i>0xfffff)
                {
                        say("data not ready!\n");
                        return;
                }

                temp=in8(command+7);
                if( (temp&0x80) == 0x80 )continue;                      //busy
                if( (temp&0x8) == 0 )continue;                  //DRQ

                break;
                //if( (temp&0x1) == 0x1 )
                //{
                //      say("read error:%x\n",temp);
                //      return;
                //}
                //else break;
        }

        //read data
        say("reading data\n");
        for(i=0;i<0x200;i+=2)
        {
                temp=in8(command+7);
                //if( (temp&1) == 1)break;
                if( (temp&0x8) != 8)break;

                temp=in16(command+0);
                dest[i]=temp&0xff;
                dest[i+1]=(temp>>8)&0xff;
        }

        if(i < 0x200) say("not finished:%x\n",i);
}
int identifyide(u64 rdi)
{
        u64 command=*(u64*)(idehome+0x10);              //command
        u64 control=*(u64*)(idehome+0x20);              //control
        u16* dest=(u16*)rdi;
        volatile int i;
        volatile u32 temp=0;

        //channel reset
        out8(control+2,4);
        out16(0x80,0x1111);             //out8(0xeb,0)
        say("out %x %x\n",control+2,4);

        out8(control+2,0);
        say("out %x %x\n",control+2,0);

        out8(command+6,0xa0);
        say("out %x %x\n",command+6,0xa0);

        //50:hd exist
        i=0;
        while(1)
        {
                i++;
                if(i>0xfffff)
                {
                        say("device not ready:%x\n",temp);
                        return -1;
                }

                temp=in8(command+7);
                if( (temp&0x80) == 0x80 )continue;                      //busy
                if( (temp&0x40) == 0x40 )break;                 //Device ReaDY?
        }

        //count
        out8(command+2,0);
        out8(command+3,0);
        out8(command+4,0);
        out8(command+5,0);

        //mode
        out8(command+6,0xe0);
        say("out %x %x\n",command+6,0xe0);

        //start reading
        out8(command+7,0xec);
        say("out %x %x\n",command+7,0xec);

        //check hd data ready?
        i=0;
        while(1)
        {
                i++;
                if(i>0xfffff)
                {
                        say("data not ready!\n");
                        return -2;
                }

                temp=in8(command+7);
                if( (temp&0x80) == 0x80 )continue;                      //busy
                if( (temp&0x8) == 0 )continue;                  //DRQ

                break;
        }

        //read data
        say("reading data\n");
        for(i=0;i<0x100;i++)
        {
                temp=in8(command+7);
                //if( (temp&1) == 1)break;
                if( (temp&0x8) != 8)break;

                temp=in16(command+0);           //只管放，后面再调字节序
                dest[i]=temp;
        }

        if(i < 0x100) say("not finished:%x\n",i);
        return 1;
}




static void probepci(u64 addr)
{
//?：pci地址
//出：?存地址
	u32 temp;
	u32 bar0,bar1;
	say("(ide)pciaddr:%x{\n",addr);

	out32(0xcf8,addr+0x4);
	temp=in32(0xcfc)|(1<<10)|(1<<2);		//bus master=1

	out32(0xcf8,addr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,addr+0x4);
	temp=(u64)in32(0xcfc);
	say("    sts&cmd:%x",temp);

	//ide port
	out32(0xcf8,addr+0x10);
	bar0=in32(0xcfc)&0xfffffffe;
	say("    (command)bar0:%x\n",bar0);

	out32(0xcf8,addr+0x14);
	bar1=in32(0xcfc)&0xfffffffe;
	say("    (control)bar1:%x\n",bar1);

	out32(0xcf8,addr+0x18);
	temp=in32(0xcfc)&0xfffffffe;
	say("    bar2:%x\n",temp);

	out32(0xcf8,addr+0x1c);
	temp=in32(0xcfc)&0xfffffffe;
	say("    bar3:%x\n",temp);

	//找到了，放到自己的表格里
	say("ide,%x,%x",bar0,bar1);
	say("}\n");
}
void initide(u64 pciaddr)
{
	u64 addr;
	say("ide@%x", pciaddr);

	//probe pci
	probepci(pciaddr);
}

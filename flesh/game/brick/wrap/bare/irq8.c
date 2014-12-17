static inline void out8( unsigned short port, unsigned char val )
{
    asm volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}
static inline unsigned char in8( unsigned short port )
{
    unsigned char ret;
    asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}
static void isr8()
{
	(*(unsigned long long*)0x7f8) += 1;
	asm("int3");
}


void enableidt(int irqnum,unsigned long long isr)
{
	unsigned char* rdi=(unsigned char*)(unsigned long long)(0x1200+irqnum*0x10);
	rdi[0]=isr&0xff;
	rdi[1]=(isr>>8)&0xff;
	rdi[2]=8;
	rdi[3]=0;
	rdi[4]=0;
	rdi[5]=0x8e;
	rdi[6]=(isr>>16)&0xff;
	rdi[7]=(isr>>24)&0xff;
	rdi[8]=isr>>32;
	rdi[9]=isr>>40;
	rdi[10]=isr>>48;
	rdi[11]=isr>>56;
	rdi[12]=0;
	rdi[13]=0;
	rdi[14]=0;
	rdi[15]=0;
}
enableapic()
{
	unsigned int* addr=(unsigned int*)0xfec00000;
	unsigned int* data=(unsigned int*)0xfec00010;
	addr[0]=0x10+     2*8;
	data[0]=0x28;
	addr[0]=0x10+1+   2*8;
	data[0]=0;
}
void enablecmos()
{
	out8(0x70,0x8a);
	out8(0x71,0xac);		//3=8192hz,6=1024hz,c=8hz,f=1hz

	out8(0x70,0x8b);
	unsigned char temp=in8(0x71);

	out8(0x70,temp);
	out8(0x71,temp|0x40);
}
void enableirq8()
{
	//时间中断
	*(unsigned long long*)0x7f8=0;
	enableidt(8,(unsigned long long)isr8);
	enableapic();
	enablecmos();
}


void disablecmos()
{

}
void disableapic()
{

}
void disableidt(int irqnum,unsigned long long isr)
{
	unsigned long long* rdi=(unsigned long long*)(unsigned long long)(0x1200+irqnum*0x10);
	rdi[0]=0;
	rdi[1]=0;
}
void disableirq8()
{
	disablecmos();
	disableapic();
	disableidt(8,(unsigned long long)isr8);
}
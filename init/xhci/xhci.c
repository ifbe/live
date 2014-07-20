#define QWORD unsigned long long
#define xhcihome 0x200000



unsigned int findaddr()
{
        QWORD addr=0x5004;
        unsigned int temp;
        for(;addr<0x6000;addr+=0x10)
        {
                temp=*(unsigned int*)addr;
                temp&=0xffffff00;
                if(temp==0x0c033000)
                {
                        addr+=4;
                        temp=*(unsigned int*)addr;
                        say("xhci(port)@",(QWORD)temp);
                        return temp;
                }
        }
	return 0;
}




static inline void out32( unsigned short port, unsigned int val )
{
    asm volatile( "outl %0, %1"
                  : : "a"(val), "Nd"(port) );
}
static inline unsigned int in32( unsigned short port )
{
    unsigned int ret;
    asm volatile( "inl %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}
unsigned int probepci(unsigned int addr)
{
        out32(0xcf8,addr+0x4);
        unsigned int temp=in32(0xcfc)|(1<<10);
        out32(0xcf8,addr+0x4);
        out32(0xcfc,temp);

        out32(0xcf8,addr+0x4);
        //say("pci status and command:",(QWORD)in32(0xcfc));

        out32(0xcf8,addr+0x10);
        addr=in32(0xcfc)&0xfffffff0;
        say("xhci@",(QWORD)addr);

        int i=0;
        unsigned long long* table=(unsigned long long*)0x5000;
        for(i=0;i<0x200;i+=2)
        {
                if(table[i]==0){
                        table[i]=0x69636878;
                        table[i+1]=addr;
                        break;
                }
        }
        return addr;
}




void initxhci()
{
        unsigned int addr;

        addr=findaddr();                //port addr of port
        if(addr==0) return;

        addr=probepci(addr);            //memory addr of ahci
        if(addr==0) return;

	say("",0);
}

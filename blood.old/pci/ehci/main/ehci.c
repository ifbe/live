#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

static QWORD ehciport[2];
static QWORD ehciaddr[2];
static int hccount;




void findehci()
{
        QWORD addr=0x4004;
        DWORD temp;
	ehciport[0]=ehciport[1]=0;
	hccount=0;

        for(;addr<0x5000;addr+=0x10)
        {
                temp=*(DWORD*)addr;
                temp&=0xffffff00;
                if(temp==0x0c032000)
                {
                        ehciport[hccount]=*(DWORD*)(addr+4);
                        say("ehci(port)@",ehciport[hccount]);
			hccount++;
                }
        }
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
QWORD ehcipci(int which)
{
	//disable pin interrupt+enable bus mastering
	//very important,in qemu-kvm 1.6.2,bus master bit is 0,must set 1
        out32(0xcf8,ehciport[which]+0x4);
        DWORD temp=in32(0xcfc)|(1<<10)|(1<<2);
        out32(0xcf8,ehciport[which]+0x4);
        out32(0xcfc,temp);

        out32(0xcf8,ehciport[which]+0x4);
        say("pci status and command:",(QWORD)in32(0xcfc));

	//deal with capability list
	//explaincapability();

	//get ehciaddr from bar0
        out32(0xcf8,ehciport[which]+0x10);
        ehciaddr[which]=in32(0xcfc)&0xfffffff0;
        say("ehci@",ehciaddr[which]);

        int i=0;
        QWORD* table=(QWORD*)0x4000;
        for(i=0;i<0x200;i+=2)
        {
                if(table[i]==0){
                        table[i]=0x69636865;
                        table[i+1]=ehciaddr[which];
                        break;
                }
        }
}




void initehci()
{
	//find pci address
        findehci();
        if(ehciport==0) goto end;

	//pci部分
	int which;
	for(which=0;which<hccount;which++)
	{
		ehcipci(which);
		if(ehciaddr[which]==0) goto end;
	}

	end:
	say("",0);

}

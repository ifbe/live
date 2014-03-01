#define signed64 long long

static inline void outb( unsigned short port, unsigned char val )
{
    asm volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline unsigned char inb( unsigned short port )
{
    unsigned char ret;
    asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

void start()
{
    char in;
    char* rdi=(char*)0xa0000;
    while(in!=0x39)
    {
        in=inb(0x60);
    }
    for(;(signed64)rdi<0xaf9ff;rdi++){*rdi=0x8;}  //0x0~0xaf9f

    hlt:asm("hlt");
    goto hlt;
}

#define signed64 long long

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

void main()
{
    char in;
    char* rdi=(char*)0xa0000;
    while(in|=0x39)
    {
        in=in8(0x60);
    }
    for(;(signed64)rdi<0xaf9ff;rdi++){*rdi=0x8;}  //0x0~0xaf9f

    hlt:asm("hlt");
    goto hlt;
}

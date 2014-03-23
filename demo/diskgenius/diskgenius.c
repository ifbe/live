#include "define.h"

int where=0;


void point(int x,int y,int color)
{
    QWORD* video=(QWORD*)0x3028;
    QWORD base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=color;
}
void anscii(int x,int y,char ch)
{
    int i,j;
    QWORD points=0xb000;
    char temp;
    char* p;

    if(ch<0)ch=0x20;
    points+=ch<<4;
    x=8*x;
    y=16*y;

    for(i=0;i<16;i++)
    {
        p=(char*)points;
        for(j=0;j<8;j++)
        {
            temp=*p;
            temp=temp<<j;
            temp&=0x80;
            if(temp!=0){point(j+x,i+y,0xffffffff);}
            else{point(j+x,i+y,0);}

        }
    points++;
    }
}
void say(char* p,QWORD z)
{
	int x=0;
	while(*p!='\0')
	{
		anscii(x,where,*p);
		p++;
		x++;
	}

        int i=0;
	char ch;
        for(i=15;i>=0;i--)
        {
        ch=(char)(z&0x0000000f);
        z=z>>4;
        anscii(32+i,where,ch);
        }

	where++;
	if(where==50)where=0;
}





 

QWORD finddisk()
{
        QWORD addr=0x6008;
        unsigned int temp;
        for(;addr<0x6200;addr+=0x10)
        {
                temp=*(unsigned int*)addr;
                if(temp==0x61746173)
                {
                        addr-=0x8;
                        temp=*(unsigned int*)addr;
                        say("sata address:",(QWORD)temp);
                        return (QWORD)temp;
                }
        }
	return 0;
}





void main()
{
	BYTE* memory=(BYTE*)0;
	QWORD addr;
	int x,y,rsi;

	addr=finddisk();
	if(addr==0)return;

	identify(0x100000,addr);
/*
        for(rsi=0x100000;rsi<0x100080;rsi+=2){
		y=where+(rsi-0x100000)/0x20;
		x=2*((rsi-0x100000)%0x20);

	        anscii(  x,y,(memory[rsi]>>4)&0xf);
	        anscii(x+1,y,memory[rsi]&0xf);
	        anscii(x+2,y,(memory[rsi+1]>>4)&0xf);
	        anscii(x+3,y,memory[rsi+1]&0xf);

		x=0x42+2*rsi%0x40;

	        anscii(  x,y,memory[rsi+1]);
	        anscii(x+1,y,memory[rsi]);
        }
	where+=4;
*/

	read(0x100000,0,addr,8);

/*
        for(rsi=0x100000;rsi<0x100200;rsi++){
		y=where+(rsi-0x100000)/0x20;
		x=2*((rsi-0x100000)%0x20);

		anscii(  x,y,(memory[rsi]>>4)&0xf);
		anscii(x+1,y,memory[rsi]&0xf);

		x=0x41+2*rsi%0x40;

		anscii(x,y,memory[rsi]);
        }
	where+=16;
*/

	if(*(WORD*)0x1001fe==0xAA55)say("normal disk",0);
}

#include "define.h"

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

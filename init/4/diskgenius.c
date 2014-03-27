#include "struct.h"

QWORD finddisk()
{
        QWORD addr=0x2808;
        unsigned int temp;
        for(;addr<0x3000;addr+=0x10)
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

	read(0x100000,0,addr,2);

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

	if(*(WORD*)0x1001fe==0xAA55)say("good disk");
	else{say("bad disk");return;}

	QWORD offset;
	if(*(QWORD*)0x100200==0x5452415020494645)
	{
		say("gpt part");
		read(0x100000,2,addr,32);
		for(offset=0x100000;offset<0x104000;offset+=0x80)    //get esp
		{
			if( *(QWORD*)offset==0x11d2f81fc12a7328 ){
				if( *(QWORD*)(offset+8)==0x3bc93ec9a0004bba ){
					offset=*(QWORD*)(offset+0x20);
					break;
				}
			}
		}
		if(offset=0x104000)
		{
			say("no esp");
			return;
		}
	}
	else{
		say("mbr disk");
		for(offset=0x1000be;offset<0x1000ee;offset+=0x10)    //get fat?
		{
			if( *(BYTE*)(offset+4)==0x7 | *(BYTE*)(offset+4)==0xb)
			{
				offset=(QWORD)(*(DWORD*)(offset+8));
				break;
			}
		}
		if(offset=0x1000ee)
		{
			say("no fat32");
			return;
		}
	}

	read(0x100000,offset,addr,1);
	say("name:",*(QWORD*)0x100003);

}

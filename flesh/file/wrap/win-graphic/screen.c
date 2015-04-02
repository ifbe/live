#include <stdio.h>
#include <stdlib.h>


int width=1024;
int height=768;
unsigned int* mypixel;


__attribute__((constructor)) void initpixel()
{
	mypixel=(unsigned int*)malloc(width*height*4);
}
__attribute__((destructor)) void destorypixel()
{
	free(mypixel);
}




//inner
//outer




void point(int x,int y,int color)
{
	mypixel[y*width+x]=color;
}
void draw(int x,int y,int color)
{
	point(x+(width/2),(height/2)-y-1,color);
}
void anscii(int x,int y,char ch)
{
    int i,j;
    unsigned long long source=0x5000;
    char temp;
    char* p;

    if(ch<0)ch=0x20;
    source+=ch<<4;
    x=8*x;
    y=16*y;

    for(i=0;i<16;i++)
    {
        p=(char*)source;
        for(j=0;j<8;j++)
        {
            temp=*p;
            temp=temp<<j;
            temp&=0x80;
            if(temp!=0){point(j+x,i+y,0xffffffff);}
            else{point(j+x,i+y,0);}

        }
    source++;
    }
}
void say(char* p,...)
{
	register unsigned long long rsi asm("rsi");
	unsigned long long first=rsi;
	char* journal=(char*)0x40000;
	int x=0;
	int y=*(int*)0x7fff8;
	int screeny=y%48;
	//for(x=0;x<64;x++) anscii(x,screeny,0x20);

	x=0;

	if(y>=0xfff | y<0){
		y=0;
		*(int*)0x7fff8=0;
	}
	else *(int*)0x7fff8=y+1;

	while(*p!='\0')
	{
		//anscii(x,screeny,*p);
		journal[y*64+x]=*p;
		p++;
		x++;
	}

	if(first!=0)
	{
	        int i=0;
		char ch;
		int signal=0;

	        //for(i=0;i<16;i++) anscii(x+i,screeny,0x20);

	        for(i=0;i<16;i++)
	        {
	        	ch=( first >> (60-4*i) ) & 0xf;
			if(ch != 0) signal++;
			else if(signal) signal++;

			if(signal !=0)
			{
				ch+=0x30;
				if(ch>0x39) ch+=0x7;
	        		//anscii(x+signal-1,screeny,ch);
				journal[y*64+x+signal-1]=ch;
			}
	        }
	}
}
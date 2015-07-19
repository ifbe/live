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

	if(first==0) return;

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

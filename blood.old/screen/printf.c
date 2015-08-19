void printf(char* p,...)
{
        register unsigned long long rsi asm("rsi");
	unsigned long long first=rsi;
	char* console=(char*)0x120000;
	unsigned int x=0;
	unsigned int y=0;

	//终端换行
	y=*(unsigned int*)0x12fff8;
        if( y < 0x80*47 )
	{
		y+=128;
		*(unsigned int*)0x12fff8=y;
	}
	else
	{
		char* source=(char*)0x120080;
        	char* dest=(char*)0x120000;
		int i;
		for(i=0;i<128*0x30;i++)
		{
			dest[i]=source[i];
		}
	}

	while(*p!='\0')
	{
		console[y+x]=*p;
		p++;
		x++;
	}

	if(first!=0)
	{
	        int i=0;
		char ch;
		int signal=0;

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
				console[y+x+signal-1]=ch;
			}
	        }
	}
}

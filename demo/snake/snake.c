#define u64 long long


void point(int x,int y,int z)
{
    u64* video=(u64*)0x3028;
    u64 base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=z;
}


void anscii(int x,int y,char ch)
{
    int i,j;
    u64 points=0x7000;
    char temp;
    char* p;

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
            if(temp!=0){
                point(x+j,y+i,0xffffffff);
            }

            /*
            else{
                point(x+j,y+i,0);
            }
            */

        }
    points++;
    }
}


void decimal(int x,int y,u64 z)
{
        char ch;
        int i=0;
        u64 temp;

        if(z<0){
        anscii(x,y,'-');
        x++;
        z=-z;
        }

        temp=z;
        while(temp>=10){
        temp=temp/10;
        i++;
        }

        for(;i>=0;i--)
        {
        ch=(char)(z%10);
        z=z/10;
        anscii(x+i,y,ch);
        }
}


int random()
{
        int key,i=0;
        char* memory=(char*)0x0;
        for(i=0;i<0x1000;i++)
                key+=memory[i];
        return key;
}


void cubie(x,y)
{
	int i,j;
	for(i=16*x;i<16*(x+1);i++)
		for(j=16*y;j<16*(y+1);j++)
			point(i,j,0xffffffff);

	for(i=16*x;i<16*(x+1);i++){
		point(i,16*y,0);
		point(i,16*(y+1),0);
	}
	for(j=16*y;j<16*(y+1);j++){
		point(16*x,j,0);
		point(16*(y+1),j,0);
	}
}


void main()
{
	int i,j;
	struct table{
		int x;
		int y;
		int direction;
	}snake[11];

	for(i=0;i<10;i++)
	{
		snake[i].x=i+54;
		snake[i].y=0;
		snake[i].direction=2;
	}
	snake[i].direction=0;

	while(1)
	{
		for(i=0;i<1024;i++)		//clear screen
			for(j=0;j<768;j++)
				point(i,j,0);

		i=0;
		while( snake[i].direction != 0 )
		{
			cubie(snake[i].x,snake[i].y,0xffffffff);
			i++;
		}

		//waitforsometime
		asm("hlt");

		i=0;
		while( snake[i].direction !=0 )
		{
			switch(snake[i].direction)
			{
				case 1:{
					snake[i].x--;
					break;
				}
				case 2:{
					snake[i].x++;
					break;
				}
				case 3:{
					snake[i].y--;
					break;
				}
				case 4:{
					snake[i].y++;
					break;
				}
			}

			i++;
		}
	}
}


void int21()		//keyboard
{
		char key=keyboard();
		if(key==0x4b)	//left
		{
		}

		if(key==0x4d)	//right
		{
		}

		if(key==0x48)	//up
		{
		}

		if(key==0x50)	//down
		{
}		}

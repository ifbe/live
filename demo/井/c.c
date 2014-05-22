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
    u64 points=0x6000;
    char temp;
    char* p;

    points+=ch<<4;
    x=16*x;
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
                point(x+2*j,y+2*i,0xffffffff);
                point(x+2*j+1,y+2*i,0xffffffff);
                point(x+2*j,y+2*i+1,0xffffffff);
                point(x+2*j+1,y+2*i+1,0xffffffff);
            }

            else{
                point(x+2*j,y+2*i,0);
                point(x+2*j,y+2*i+1,0);
                point(x+2*j+1,y+2*i,0);
                point(x+2*j+1,y+2*i+1,0);
            }

        }
    points++;
    }
}


void string(int x,int y,char* p)
{
    while(*p!='\0')
    {
    anscii(x,y,*p);
    p++;
    x++;
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



double cosine(double x)
{
    double ret=0,item=1.0,temp;
    int n=0,i,sign=1;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        temp=item;
        for(i=1;i<=2*n;i++)temp/=i;
        ret+=sign*temp;
        item*=x*x;
        sign *=-1;
        n++;
      }while (temp>1e-10);
return ret;
}


double sine(double x)
{
    int m = 1,i;
    double temp,ret = 0.0;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        i=0;
        if (m%2 == 0){temp= -1.0;}
        else{temp= 1.0;}
        for(i=1;i<=2*m-1;i++){temp = temp * x/i;}
        ret+= temp;
        m++;
    }while (temp<-.0000005||temp>0.0000005);
return ret;
}


void cubie(int x,int y,int z)
{
	int i,j;

	//背景色
	for(i=256*x;i<256*x+256;i++)
		for(j=256*y;j<256*y+256;j++)
			point(i,j,0x44444444);

	for(i=256*x;i<256*x+256;i++)
	{
		point(i,256*y,0);
		point(i,256*y+255,0);
	}
	for(j=256*y;j<256*y+256;j++)
	{
		point(256*x,j,0);
		point(256*x+255,j,0);
	}

	//ooxx
	if(z==1)
	{
		double a;
		for(a=0;a<6.28;a+=0.01)
		{
		point(256*x+128+100*cosine(a),256*y+128+100*sine(a),0xff0000);
		point(256*x+129+100*cosine(a),256*y+128+100*sine(a),0xff0000);
		}
	}
	if(z==2)
	{
		for(i=28;i<228;i++)
		{
			point(256*x+i,256*y+i,0xff);
			point(256*x+i+1,256*y+i,0xff);
			point(256*x+i,256*y+256-i,0xff);
			point(256*x+i+1,256*y+256-i,0xff);
		}
	}
}


void kuang(x,y,color)
{
	int i,j;
	for(i=x*256;i<x*256+256;i++)
	{
		point(i,y*256,color);
		point(i,y*256+1,color);
		point(i,y*256+254,color);
		point(i,y*256+255,color);
	}
	for(j=y*256;j<y*256+256;j++)
	{
		point(x*256,j,color);
		point(x*256+1,j,color);
		point(x*256+254,j,color);
		point(x*256+255,j,color);
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


int check(int table[3][3])
{
	if(table[0][0]==table[0][1]&&table[0][1]==table[0][2])
	{
		if(table[0][0]==1)	return 1;
		if(table[0][0]==2)	return 2;
	}
	if(table[1][0]==table[1][1]&&table[1][1]==table[1][2])
	{
		if(table[1][0]==1)	return 1;
		if(table[1][0]==2)	return 2;
	}
	if(table[2][0]==table[2][1]&&table[2][1]==table[2][2])
	{
		if(table[2][0]==1)	return 1;
		if(table[2][0]==2)	return 2;
	}
	if(table[0][0]==table[1][0]&&table[1][0]==table[2][0])
	{
		if(table[0][0]==1)	return 1;
		if(table[0][0]==2)	return 2;
	}
	if(table[0][1]==table[1][1]&&table[1][1]==table[2][1])
	{
		if(table[0][1]==1)	return 1;
		if(table[0][1]==2)	return 2;
	}
	if(table[0][2]==table[1][2]&&table[1][2]==table[2][2])
	{
		if(table[0][2]==1)	return 1;
		if(table[0][2]==2)	return 2;
	}
	if(table[0][0]==table[1][1]&&table[1][1]==table[2][2])
	{
		if(table[1][1]==1)	return 1;
		if(table[1][1]==2)	return 2;
	}
	if(table[0][2]==table[1][1]&&table[1][1]==table[2][0])
	{
		if(table[1][1]==1)	return 1;
		if(table[1][1]==2)	return 2;
	}

	if(	table[0][0]!=0&&table[0][1]!=0&&table[0][2]!=0
	&&	table[1][0]!=0&&table[1][1]!=0&&table[1][2]!=0
	&&	table[2][0]!=0&&table[2][1]!=0&&table[2][2]!=0)
	{return 3;}

	return 0;
}


void main()
{
	int x1,y1,x2,y2;
	int table[3][3];
	int i,j;

	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			table[i][j]=0;

	x1=y1=0;
	x2=y2=2;

	for(i=0;i<1024;i++)
		for(j=0;j<768;j++)
			point(i,j,0);

	char key=0;
	while(1)
	{
		for(i=0;i<3;i++)
			for(j=0;j<3;j++)
				cubie(i,j,table[j][i]);
		kuang(x1,y1,0xff0000);
		kuang(x2,y2,0xff);

		key=keyboard();

		if(key==0x1){break;}
		if(key==0x1e){if(x1>0) x1--;}
		if(key==0x20){if(x1<2) x1++;}
		if(key==0x11){if(y1>0) y1--;}
		if(key==0x1f){if(y1<2) y1++;}
		if(key==0x4b){if(x2>0) x2--;}
		if(key==0x4d){if(x2<2) x2++;}
		if(key==0x48){if(y2>0) y2--;}
		if(key==0x50){if(y2<2) y2++;}
		if(key==0x39)
		{
			if(table[y1][x1] == 0){	//落子无悔
				table[y1][x1]=1;
			}
			int temp=check(table);
			if(temp==1){string(50,0,"red win !!!!");}
			if(temp==3){string(50,0,"no winner");}
		}
		if(key==0x1c)
		{
			if(table[y2][x2] == 0){	//落子无悔
				table[y2][x2]=2;
			}
			int temp=check(table);
			if(temp==2){string(50,0,"blue win !!!!");}
			if(temp==3){string(50,0,"no winner");}
		}

	}
}

static void directidentify()
{
	identify(0x400000,getdisk());

	unsigned char* p=(unsigned char*)0x400000;
	unsigned char temp;
	int i;
	for(i=0;i<0x100;i+=2)
	{
		temp=p[i];
		p[i]=p[i+1];
		p[i+1]=temp;
	}
}


static void directread(unsigned long long sector)
{
	int result;
	result=read(0x400000,sector,getdisk(),8);
	if(result>=0) say("read sector:",sector);
	else say("something wrong:",sector);
}


void initsata()
{
        unsigned long long* pointer=(unsigned long long*)0x4000;
        int i;
        for(i=0;i<0x200;i+=2)
        {
        if( (pointer[i]==0) | (pointer[i]==0x796669746e656469) )
        {
                pointer[i]=0x796669746e656469;
                pointer[i+1]=(unsigned long long)directidentify;
                pointer[i+2]=0x64616572;
                pointer[i+3]=(unsigned long long)directread;
                break;
        }
        }
}

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


//ϵͳ���߸��ֶ����ṩ�õ�memory���⼸������������¼��λ��
static QWORD readbuffer;
static QWORD directorybuffer;
static QWORD catalogbuffer;
void printmemory(QWORD addr,QWORD size);





static void hfs_explain()
{
	
}


static void hfs_cd()
{

}


static void hfs_load()
{

}

int mounthfs(QWORD sector,QWORD* explainfunc,QWORD* cdfunc,QWORD* loadfunc)
{
	//����cd��load�����ĵ�ַ
	*explainfunc=(QWORD)hfs_explain;
	*cdfunc=(QWORD)hfs_cd;
	*loadfunc=(QWORD)hfs_load;

	//׼���ÿ��õ��ڴ��ַ
	getaddrofbuffer(&readbuffer);
	getaddrofdir(&directorybuffer);
	getaddroffs(&catalogbuffer);

	//������ǰ8�������ܹ�0x1000�ֽ�(��ʵֻҪ������2�ź�3������)
	read(readbuffer,sector,0,0x8);	//0x1000
	printmemory(readbuffer+0x400,0x200);

	//���magicֵ
	if( *(WORD*)(readbuffer+0x400) == 0x2b48 )
	{
		say("hfs+\n");
	}
	else if( *(WORD*)(readbuffer+0x400) == 0x5848 )
	{
		say("hfsx\n");
	}

	return 0;
}

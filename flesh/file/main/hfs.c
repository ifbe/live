#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


//ϵͳ���߸��ֶ����ṩ�õ�memory���⼸������������¼��λ��
static QWORD directorybuffer;
static QWORD readbuffer;
static QWORD mftbuffer;
static QWORD indexbuffer;


void checkcacheforwhat()
{
	
}


void hfs_cd()
{

}


void hfs_load()
{

}

int mounthfs(QWORD sector,QWORD* cdfunc,QWORD* loadfunc)
{
	//����cd��load�����ĵ�ַ
	*cdfunc=(QWORD)hfs_cd;
	*loadfunc=(QWORD)hfs_load;

	return 0;
}
void main();

void start()
{
//	char* p=(char*)0x10000;
//	for(;p<(char*)0x40000;p++){*(p+0xf0000)=*p;}
//	void (*f)()=main;	//program moved
//	f();			//should call right address
	main();
}

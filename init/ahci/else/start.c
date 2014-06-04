void start()
{
	char* memory=(char*)0x40000;
	int i;
	for(i=0;i<0x40000;i++) memory[i]=0;

	initahci();
	initsata();
	parttable();
}

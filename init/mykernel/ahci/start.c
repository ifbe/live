
void start()
{
	char* memory=(char*)0;
	int i;
	for(i=0x8000;i<0x10000;i++) memory[i]=0;

	initahci();

        finddisk();

	parttable();

	mount();
}

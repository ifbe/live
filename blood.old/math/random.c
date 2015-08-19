int random()
{
	int key,i=0;
	char* memory=(char*)0x800;
	for(i=0;i<0x800;i++)
		key+=memory[i];
	return key;
}
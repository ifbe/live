unsigned char* tablebuf;
unsigned char* readbuf;
__attribute__((constructor)) void initmemory()
{
        tablebuf=(unsigned char*)malloc(0x10000);
        readbuf=(unsigned char*)malloc(0x100000);
}
__attribute__((destructor)) void destorymemory()
{
        free(tablebuf);
        free(readbuf);
}




void getaddroftable(unsigned long long* p)
{
	*p=tablebuf;
}
void getaddrofbuffer(unsigned long long* p)
{
	*p=readbuf;
}

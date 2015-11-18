static unsigned long long offset=0x800000;


void* getfreememory(int i)
{
    void* p=(void*)offset;
    offset+=i;
    return p;
}

void main()
{
    unsigned long long jpegbase=where();
    int sz=size();
    int i;
    i=njDecode((char*)jpegbase,&sz);

    if(i==0) picture2screen();
}

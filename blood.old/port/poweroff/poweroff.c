#define wheretogetport 0x4fc
#define wheretogetdata 0x4fe
void turnoff()
{
    short port=  *(short*)(wheretogetport);
    short data=( *(short*)(wheretogetdata) ) | 0x2000;
    outw(port,data);
}

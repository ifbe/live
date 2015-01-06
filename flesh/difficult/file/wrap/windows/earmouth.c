#include<stdio.h>
void listen(char* p)
{
	int i;
	for(i=0;i<128;i++)p[i]=0;
	gets(p);
}
/*
void say(char* string,unsigned long long data)
{
	printf("%s",string);
	if(data!=0)printf("%llx",data);
	printf("\n");
}*/
void say(char* p,...)
{
	asm("jmp printf");
}
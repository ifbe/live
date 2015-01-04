#include<stdio.h>
void say(char* string,unsigned long long data)
{
	printf("%s",string);
	if(data!=0)printf("%llx",data);
	printf("\n");
}

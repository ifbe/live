#include <stdio.h>
#include <stdlib.h>


unsigned int* mycoordinate;


__attribute__((constructor)) void initcoordinate()
{
	//准备framebuffer
	mycoordinate=(unsigned int*)malloc(256*256*256*4);
}
__attribute__((destructor)) void destorycoordinate()
{
	//释放framebuffer
	free(mycoordinate);
}




//inner
//outer




void coordinate(int x,int y,int z,int color)
{
	mycoordinate[z*256*256+y*256+x]=color;
}
unsigned int readcoordinate(int x,int y,int z)
{
	return mycoordinate[z*256*256+y*256+x];
}
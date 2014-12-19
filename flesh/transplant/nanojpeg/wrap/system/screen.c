#include <stdio.h>
#include <stdlib.h>
unsigned int* mypixel;
int width=1024;
int height=768;


__attribute__((constructor)) void initmemory()
{
	//准备framebuffer
	mypixel=(unsigned int*)malloc(width*height*4);
}
__attribute__((destructor)) void destorymemory()
{
	//释放framebuffer
	free(mypixel);
}




void point(int x,int y,int color)
{
	mypixel[y*width+x]=color;
}
void draw(int x,int y,int color)
{
	point(x+(width/2),(height/2)-y-1,color);
}
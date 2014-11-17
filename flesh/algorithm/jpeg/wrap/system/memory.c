#include<stdlib.h>
void initmemory (void) __attribute__ ((constructor));
void destorymemory (void) __attribute__ ((destructor));


extern unsigned int* mypixel;
extern int width;
extern int height;


void initmemory()
{
	//准备framebuffer
	mypixel=(unsigned int*)malloc(width*height*4);
}


void destorymemory()
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








void* mymalloc(int size)
{
	return malloc(size);
}

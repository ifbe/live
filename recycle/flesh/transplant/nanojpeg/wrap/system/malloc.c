#include<stdlib.h>


//不来这么一层没法保证main里面的函数不用#include <stdlib.h>
void* getfreememory(int size)
{
	return malloc(size);
}
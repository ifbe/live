#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"




//上面几个负责初始化
//下面几个提供使用




int waitevent()
{
	SDL_Event event;
	while (1)
	{
	//if (SDL_PollEvent(&event))
	//{
	if (SDL_WaitEvent(&event))
	{
		if (event.type == SDL_QUIT) {
			return -1;
		}
       		if (event.type == SDL_KEYDOWN) {
			//printf("%x\n",(int)(event.key.keysym.sym));
			return (int)(event.key.keysym.sym);
		}
        }
	}
}

#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
SDL_TimerID my_timer_id;


unsigned int my_callbackfunc(unsigned int interval, void *param)
{
    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0xff;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);

	return interval;
}
__attribute__((constructor)) void inittimer()
{
	unsigned int delay =(33 / 10) * 20;  /* To round it down to the nearest 10 ms */
	my_timer_id = SDL_AddTimer(delay, my_callbackfunc,0);
}
__attribute__((destructor)) void destorytimer()
{}



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
		if (event.type == SDL_USEREVENT) {
			return (int)event.user.code;
		}
       	if (event.type == SDL_KEYDOWN) {
			//printf("%x\n",(int)(event.key.keysym.sym));
			return (int)(event.key.keysym.sym);
		}
        }
	}
}
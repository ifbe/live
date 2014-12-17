#include "SDL2/SDL.h"
#undef main


extern int width;
extern int height;
extern unsigned int* mypixel;
SDL_Window* window;//窗口
SDL_Renderer* renderer;	//什么是render
SDL_Texture* texture;		//texture是在显存里的？
SDL_TimerID my_timer_id;


unsigned int my_callbackfunc(unsigned int interval, void *param)
{
  //push SDL_USEREVENT into queue,causes it to be called again same interval
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
__attribute__((constructor)) void initsdl()
{
return;
  //准备sdl
  SDL_Init(SDL_INIT_EVERYTHING);
  window=SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          16,
                          width,height,
                          SDL_WINDOW_OPENGL);
  renderer= SDL_CreateRenderer(window, -1, 0);
  texture= SDL_CreateTexture(renderer,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               width,height);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  //准备timer
  unsigned int delay =(33/10) * 20; //round it down to the nearest 10 ms
  my_timer_id = SDL_AddTimer(delay, my_callbackfunc,0);
}
__attribute__((destructor)) void destorysdl()
{
return;
  //释放sdl
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window); 
  SDL_Quit(); 
}




//inner
//outer




void writescreen()
{
  //画texture？
  SDL_UpdateTexture(texture, NULL, mypixel, width*4);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}
int waitevent()
{
        SDL_Event event;
        while (1)
        {
                if (SDL_WaitEvent(&event))
                {
                        if (event.type == SDL_QUIT)
                        {
                                return -1;
                        }
                        else if (event.type == SDL_KEYDOWN)
                        {
                                return (int)(event.key.keysym.sym);
                        }
                        else if (event.type == SDL_USEREVENT)
                        {
                                return (int)event.user.code;
                        }
                }
        }
}


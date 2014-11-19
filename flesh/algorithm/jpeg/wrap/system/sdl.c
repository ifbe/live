#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#undef main
void initsdl (void) __attribute__ ((constructor));
void destorysdl (void) __attribute__ ((destructor));


extern unsigned int* mypixel;
extern int width;
extern int height;


SDL_Window* window;//窗口
SDL_Renderer* renderer;	//什么是render
SDL_Texture* texture;		//texture是在显存里的？


//int SDL_main( int argc, char* args[] ) 
void initsdl()
{
  //准备sdl
  SDL_Init(SDL_INIT_EVERYTHING);
  window=SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          width,height,
                          SDL_WINDOW_OPENGL);
  renderer= SDL_CreateRenderer(window, -1, 0);
  texture= SDL_CreateTexture(renderer,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               1024, 768);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  
  
//printf("sdl inited\n");
}


void destorysdl()
{
  //释放sdl
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window); 
  SDL_Quit(); 

//printf("sdl destoryed\n");
}








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
			//printf("%x\n",(int)(event.key.keysym.scancode));
			//return event.key.keysym.scancode;
		}
        }
	}
}

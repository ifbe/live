#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL2/SDL.h"
#include <SDL2/SDL_opengl.h>
#undef main

#ifdef __APPLE__
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
#else
	#include <GL/gl.h>
#endif


SDL_Window* window;//窗口
SDL_GLContext glcontext;


__attribute__((constructor)) void initsdl()
{
	//准备sdl
	SDL_Init(SDL_INIT_EVERYTHING);

	//sdl_gl
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(1);
	window=SDL_CreateWindow("My Game Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		512,512,
		SDL_WINDOW_OPENGL);
	glcontext = SDL_GL_CreateContext(window);

	//选择投影矩阵，重置投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//设置视口，选择模型观察矩阵，重置模型观察矩阵
	gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	
	
	
	//glOrtho(-2, 2, -2,2, -10, 10);
	//gluPerspective(45.0f,1.0,0.1f,100.0f);
	//glMatrixMode(GL_MODELVIEW);
	//glViewport( 0, 0, 512, 512 );
	//glFrustum( -1, 1, (float)512/512, (float)512/512, 1, 500 );
	//gluLookAt(0.0,0.0,0,
	//0.0,0.0,50,
	//0.0,1,0.0);
}
__attribute__((destructor)) void destorysdl()
{
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  //printf("desstructsdl");
}




//inner
//outer



void writecoordinate()
{
	int x,y,z;
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);

	for(x=0;x<256;x++)
	{
		for(y=0;y<256;y++)
		{
			for(z=0;z<256;z++)
			{
				unsigned int color=readcoordinate(x,y,z);
				//printf("%f\n",(float)((color>>24)&0xff)/256);
				//glColor3f((float)((color>>24)&0xff)/256,(float)((color>>16)&0xff)/256,(float)(color&0xff)/256);
				if(color)glVertex3f((float)x/256,(float)y/256,(float)z/256);
			}
		}
	}

	glEnd();
	SDL_GL_SwapWindow(window);
}
void moveeye(int degree)
{
	//double de=(double)degree/360*2*3.141592653;
	//printf("%lf\n",de);
	//glPushMatrix();
	//gluLookAt(cos(de),sin(de),0,
	//0,0,0,
	//0,0,1);
	//glRotated(30,1,0,0);
	SDL_GL_SwapWindow(window);
	//glPopMatrix();
}
int waitevent()
{
        SDL_Event event;
        while (1)
        {
        if (SDL_WaitEvent(&event))
        {
			switch(event.type)
			{
				case SDL_QUIT:
				{
					return -1;
				}
				case SDL_KEYDOWN:
				{
					//printf("here");
					return (int)(event.key.keysym.sym);
				}
				case SDL_WINDOWEVENT:
				{
					if( event.window.event == SDL_WINDOWEVENT_RESIZED )
					{
						glViewport( 0, 0, event.window.data1, event.window.data2 );
						glLoadIdentity();
						glFrustum( -1, 1, -(float)event.window.data2/event.window.data1, (float)event.window.data2/event.window.data1, 1, 500 );
					}
					break;
				}
			}
        }
        }
}
int pollevent()
{
        SDL_Event event;
        if (!SDL_PollEvent(&event)) return 1;

        if (event.type == SDL_QUIT) {
                return -1;
        }
        if (event.type == SDL_KEYDOWN) {
                return (int)(event.key.keysym.sym);
        }
}
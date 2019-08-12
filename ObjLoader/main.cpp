// This example program is created by thecplusplusuy for demonstration purposes. It's a simple 3D model loader (wavefront (.obj)), which is capable to load materials and UV textures:
// http://www.youtube.com/user/thecplusplusguy
// Free source, modify if you want, LGPL licence (I guess), I would be happy, if you would not delete the link
// so other people can see the tutorial
// this file is third.cpp an example, how to use the class
#include <iostream>
#include "objloader.h"

using namespace std;

int shape;
objloader loader;	// create an instance of the objloader

void init() {
	glClearColor(0.8,0.8,0.8,1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45,640.0/480.0,1.0,500.0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);

	shape = loader.load("i5.obj"); // this is a hardcoded filename. Change it to your needs!

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float col[]={1.0,1.0,1.0,1.0};
	glLightfv(GL_LIGHT0,GL_DIFFUSE,col);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	float pos[]={-1.0,1.0,-2.0,1.0};

	glLightfv(GL_LIGHT0,GL_POSITION,pos);
	glTranslatef(0.0,-30.0,-100.0);
	glCallList(shape);	// and display it
}

#include <dlfcn.h>  // to make it work on mac
int main(int argc,char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
    /*
    // maybe needed unser Mac, but for Linux its poison!
    void* cocoa_lib=dlopen( "/System/Library/Frameworks/Cocoa.framework/Cocoa", RTLD_LAZY );
    void (*nsappload)(void);
    nsappload = (void(*)()) dlsym( cocoa_lib, "NSApplicationLoad");
    nsappload();
    */
	//SDL_Surface* screen=SDL_SetVideoMode(640,480,32,SDL_SWSURFACE|SDL_OPENGL);
	SDL_Window* window;
	SDL_Renderer* screen;
	window = SDL_CreateWindow("SDL_RenderClear",
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        640, 480,
                        0);
	screen = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(screen, 0, 0, 0, 255);
	SDL_RenderClear(screen);
	
	bool running=true;
	Uint32 start;
	SDL_Event event;

	init();

	while (running) {
		start=SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
			switch(event.type) {
              case SDL_QUIT:
                running=false;
                break;
			}
		}

		display();
		//SDL_GL_SwapBuffers();
		SDL_RenderPresent(screen);

		if (1000/30>(SDL_GetTicks()-start)) {
			SDL_Delay(1000/30-(SDL_GetTicks()-start));
        }
	}

	SDL_Quit();
	return 0;	
}

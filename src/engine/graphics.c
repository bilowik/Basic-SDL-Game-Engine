#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>



// Impelmentations
void Graphics_Init(Graphics* this, int winSizeY, int winSizeX, char* winTitle) {
	
	// Initialize the window and renderer
	this->_window = SDL_CreateWindow(winTitle, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, winSizeX, winSizeY,
		       	SDL_WINDOW_SHOWN);
	if(this->_window == NULL) {
		printf("Window could not be created! SDL2 Error: %s\r\n",
			       	SDL_GetError());
	}
	else {
		this->_renderer = SDL_CreateRenderer(this->_window, -1,
			       	SDL_RENDERER_ACCELERATED);
		
		if(this->_renderer == NULL) {
			printf("Renderer could not be derived from window!\
				SDL2 Error: %s\r\n", SDL_GetError());
			}
		else {
			SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 0);
		}
	} // End if(this->_window == NULL)...else 
} // End Graphics_Init



void Graphics_Destroy(Graphics* this) {
	SDL_DestroyWindow(this->_window);
	SDL_DestroyRenderer(this->_renderer);	
} // End Graphics_Destroy




void Graphics_Update(Graphics* this) {
	SDL_RenderPresent(this->_renderer);
}



void Graphics_Clear(Graphics* this) {
	SDL_RenderClear(this->_renderer);
}


void Graphics_SetResizable(Graphics *this, int resizable) {
	SDL_SetWindowResizable(this->_window, resizable);
}

/*
SDL_Event * Graphics_ProcessWindowEvents(Graphics *graphics, 
		uint32_t passedTicks) {
	SDL_Event *eventBuf = malloc(sizeof(SDL_Event) * GRAPHICS_EVENT_BUF_SIZE);
	int numEvents;
	
	numEvents = SDL_PeepEvents(&eventBuf[0],
			GRAPHICS_EVENT_BUF_SIZE,
			SDL_GETEVENT,
			SDL_WINDOWEVENT_RESIZED,
			SDL_WINDOWEVENT_RESIZED,









} // End Graphics_ProcessWindowEvents
*/

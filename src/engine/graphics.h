#ifndef SMPENGINE_GRAPHICS_H
#define SMPENGINE_GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


typedef struct Graphics Graphics;

struct Graphics {
	// Private fields
	SDL_Renderer* _renderer;
	SDL_Window* _window;


};


void Graphics_Init(Graphics* this, int winSizeY, int winSizeX, char* winTitle);
// Initializes Graphics
// Parameters:
// 	int winSizeY:	Size of the window 
// 	int winSizeX:	Size of the window

void Graphics_Destroy(Graphics* this);




void Graphics_Update(Graphics* this);
// Purpose	:	Updates the window.
// Pre		:	'Graphics' has been initialized by a
// 			constructor
// Post		:	SDL_RenderPresent is called, '_renderer' is
// 			sent. 

void Graphics_Clear(Graphics* this);
// Purpose	:	Clears the renderer
// Pre		:	'Graphics' is initialized by a constructor
// Post		:	this->_renderer is cleared.

void Graphics_SetResizable(Graphics *this, int resizable);
// Purpose	:	Allows the window to resizable
// Pre		:	Graphics is initialized.
// Post		:	THe window is now resizeable

//void Graphics_ProcessWindowEvents(Graphics *graphics, uint32_t passedTicks);
// Purpose	:	Processing window events from the SDL event queue
// Pre		:	Graphics is initialized.
// Post		:	Pulls out window events from the sdl event queue and
// 				processes them.


#endif	

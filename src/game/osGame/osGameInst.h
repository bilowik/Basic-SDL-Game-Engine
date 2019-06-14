#ifndef SMPENGINE_OSGAMEINST_H
#define SMPENGINE_OSGAMEINST_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "engine/inputHandler.h"
#include "engine/graphics.h"
#include "engine/basicPhys.h"
#include "engine/globals.h"
#include "engine/spriteRegistry.h"
#include "engine/filePathHandler.h"
#include "engine/spriteKaliDB.h"
#include "engine/world.h"
#include "engine/overlay.h"


typedef struct OsGameInst OsGameInst;

struct OsGameInst {
	
	// Primary engine structures
	SpriteRegistry **registryLayers;
	int numLayers;

	Graphics graphics;
	InputHandler input;
	Overlay overlay;


	int windowWidth;
	int windowHeight;

	uint32_t passedTicks;

	SDL_Rect camOffset;

};

int OsGameInst_Init(OsGameInst *this, int argc, char **argv, int numLayers, 
		int layerSize);
// Initializes the instance

int OsGameInst_Run(OsGameInst *this, void *args);
// Main game loop

int OsGameInst_Destroy(OsGameInst *this);





// Functions to be mapped to keys (or other inputs)

void * MouseMotionHandler(SDL_Event *e, void *instance);




void MouseCursor(void *thisPtr, uint32_t passedTime, void *args);


// Misc
float RandomInt(float min, float max);



#endif

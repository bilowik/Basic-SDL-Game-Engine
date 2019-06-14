#ifndef SMPENGINE_INSTANCE_H
#define SMPENGINE_INSTANCE_H

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


typedef struct Instance Instance;

struct Instance {
	
	// Primary engine structures
	SpriteRegistry **registryLayers;
	int numLayers;

	Graphics graphics;
	InputHandler input;
	BasicWorldPhysProps worldProps;
	World world;
	Sprite *player;
	Overlay overlay;


	int windowWidth;
	int windowHeight;
	SDL_Rect camOffset; // The offset to draw sprites, creates "scrolling"

	uint32_t passedTicks;

	uint8_t playerSpawned : 1;
};

int Instance_Init(Instance *this, int argc, char **argv, int numLayers, 
		int layerSize);
// Initializes the instance

int Instance_Run(Instance *this, void *args);
// Main game loop

int Instance_Destroy(Instance *this);





// Functions to be mapped to keys (or other inputs)

void * MouseMotionHandler(SDL_Event *e, void *instance);

void * SpawnRandomBall(SDL_Event *e, void *instance);

void * OnClick(SDL_Event *e, void *instance);

void * PanCamera(SDL_Event *e, void *instance);

void * ToggleOverlay(SDL_Event *e, void *instance);


// Player inputs
void * Input_Player_Jump(SDL_Event *e, void *instance);

void * Input_Player_MoveRight(SDL_Event *e, void *instance);
void * Input_Player_StopMoveRight(SDL_Event *e, void *instance);

void * Input_Player_MoveLeft(SDL_Event *e, void *instance);
void * Input_Player_StopMoveLeft(SDL_Event *e, void *instance);


void * SpawnPlayer(SDL_Event *e, void *instance);



// Update functions
void Disappear(void *thisPtr, uint32_t passedTime, void *args);

void MouseCursor(void *thisPtr, uint32_t passedTime, void *args);

void UpdateSpriteSections(void *thisPtr, uint32_t passedTime, void *args);

void CheckBounds(void *thisPtr, uint32_t passedTime, void *args);

// Misc
float RandomInt(float min, float max);



#endif

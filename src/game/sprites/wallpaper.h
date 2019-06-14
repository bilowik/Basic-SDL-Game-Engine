#ifndef SMPENGINE_WALLPAPER_H
#define SMPENGINE_WALLPAPER_H

#include "engine/sprite.h"
#include "engine/spriteRegistry.h"

typedef struct Wallpaper Wallpaper;

struct Wallpaper {
	Sprite _base;
};


void Wallpaper_Init(void *thisPtr, SpriteRegistry *reg, int id,
		Graphics *graphics);

void Wallpaper_Destroy(void *thisPtr);






#endif 

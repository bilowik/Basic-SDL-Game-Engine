#ifndef SMPENGINE_CROSSHAIR_H
#define SMPENGINE_CROSSHAIR_H

#include "engine/sprite.h"
#include "engine/graphics.h"
#include <stdint.h>
#include "engine/globals.h"

typedef struct Crosshair Crosshair;

struct Crosshair {
	Sprite _base;

};



void Crosshair_Init(void *thisPtr, SpriteRegistry *reg, int id, 
		Graphics *graphics);

void Crosshair_Destroy(void *thisPtr);




#endif

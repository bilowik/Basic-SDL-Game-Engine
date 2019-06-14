#ifndef SMPENGINE_CURSOR_H
#define SMPENGINE_CURSOR_H

#include "engine/sprite.h"
#include "engine/graphics.h"
#include <stdint.h>
#include "engine/globals.h"

typedef struct Cursor Cursor;

struct Cursor {
	Sprite _base;

};



void Cursor_Init(void *thisPtr, SpriteRegistry *reg, int id, 
		Graphics *graphics);

void Cursor_Destroy(void *thisPtr);




#endif

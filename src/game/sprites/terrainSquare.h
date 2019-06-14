#ifndef TERRAINSQUARE_H
#define TERRAINSQUARE_H
#include <stdint.h>
#include "engine/sprite.h"

// Forward declarations:
typedef struct Graphics Graphics;
typedef struct SpriteRegistry SpriteRegistry;


typedef struct TerrainSquare TerrainSquare;

struct TerrainSquare {
	Sprite _base;
};


void TerrainSquare_Init(void *thisPtr, SpriteRegistry *reg, int id,
		Graphics *graphics);

void TerrainSquare_Destroy(void *thisPtr);

void TerrainSquare_Update(void *this, uint32_t passedTicks, void *instance);










#endif

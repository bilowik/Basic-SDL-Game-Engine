#include "game/sprites/terrainSquare.h"
#include "engine/basicPhys.h"
#include "engine/globals.h"


void TerrainSquare_Init(void *thisPtr, SpriteRegistry *reg, int id,
		Graphics *graphics) {
	Sprite *this = thisPtr;

	Sprite_Init(this, reg, G_DEFAULT_TEXTURE_PATH, graphics);
	this->_isStatic = 1;
	

	
	this->_boundingBox.x = 0;
	this->_boundingBox.y = 0;
	this->_boundingBox.w = 64;
	this->_boundingBox.h = 64;
	this->_isVisible = 1;
}

void TerrainSquare_Destroy(void *thisPtr) {
	Sprite_Destroy(thisPtr);
}
















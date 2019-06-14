#include "game/sprites/cursor.h"


static const char _spritePath[] = "/sprites/cursor.png";

void Cursor_Init(void *thisPtr, SpriteRegistry *reg, int id, 
		Graphics *graphics) {
	Sprite *this = thisPtr;	
	
	char *path = malloc(strlen(&_spritePath[0]) + strlen(G_RESOURCE_PATH) + 1);
	strcpy(path, G_RESOURCE_PATH);
	strcat(path, &_spritePath[0]);

	Sprite_Init(this, reg, path, graphics);

	this->_spriteID = id;

	this->_boundingBox.x = 0;
	this->_boundingBox.y = 0;
	this->_boundingBox.w = 32;
	this->_boundingBox.h = 32;
	this->_isVisible = 1;
}



void Cursor_Destroy(void *thisPtr) {
	Sprite_Destroy(thisPtr);
}



	

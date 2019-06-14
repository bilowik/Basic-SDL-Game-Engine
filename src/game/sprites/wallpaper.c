#include "game/sprites/wallpaper.h"
#include "engine/globals.h"


static const char _spritePath[] = "/sprites/wallpaper.png";


void Wallpaper_Init(void *thisPtr, SpriteRegistry *reg, int id,
		Graphics *graphics) {
	Sprite *this = thisPtr;

	char *path = malloc(strlen(&_spritePath[0]) + strlen(G_RESOURCE_PATH) + 1);
	strcpy(path, G_RESOURCE_PATH);
	strcat(path, &_spritePath[0]);

	Sprite_Init(this, reg, path, graphics);

	this->_spriteID = id;
	this->_boundingBox = (SDL_Rect) {
										.x = 0,
										.y = 0,
										.w = this->_partRectArray[0].w,
										.h = this->_partRectArray[0].h
									};
	this->_isVisible = 1;
} // End Wallpaper_Iint



void Wallpaper_Destroy(void *thisPtr) {
	Sprite_Destroy(thisPtr);
} // End Wallpaper_Destroy




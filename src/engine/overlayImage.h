#ifndef SMPENGINE_OVERLAY_IMAGE_H
#define SMPENGINE_OVERLAY_IMAGE_H
#include "engine/overlayElement.h"
#include "engine/sprite.h"


typedef struct OverlayImage OverlayImage;
typedef struct _OverlayImage_ImagePart _OverlayImage_ImagePart;

struct _OverlayImage_ImagePart {
	Sprite *_sprite;
	int _offsetX;
	int _offsetY;
}; 

struct OverlayImage {
	OverlayElement _base;
	_OverlayImage_ImagePart *_images;
	int _numImageParts;
	int _sizeOfImagePartArray;
};


void OverlayImage_Init(OverlayImage *this, int width, int height, 
		int initialMaxSprites);

void OverlayImage_Destroy(OverlayImage *this);

int OverlayImage_AddSprite(OverlayImage *this, Sprite *sprite, 
		int offsetX, int offsetY);
// Purpose	:	Adds a sprite to the element using the given offset
// Pre		:	OverlayImage is initialized.
// Post		:	The sprite is added to the element if it didn't already exist.
// Returns - 1 if the sprite already exists, 0 if it doesn't.
// SpritePosUpdate	:	Function that calls the give pos update function given
// 						in this file

int OverlayImage_RemoveSprite(OverlayImage *this, Sprite *sprite);
// Purpose	:	Removes the specified sprite from the element.
// Pre		:	OverlayImage is initialized.
// Post		:	The sprite has been removed from the element if it existed.
// Returns	:	-1 if the sprite didn't exist, 0 if it did and was removed.

int OverlayImage_FindSprite(OverlayImage *this, Sprite *sprite, int *index);
// Purpose	:	Finds the specified sprite if it exist puts it's index into
// 				index.
// Pre		:	OverlayImage is initialized.
// Post		:	0 is returned if the sprite is not found, 
// 				1 is returned if the sprite was found, and the index
// 				is copied into index.
// Args		:	index: Can be NULL and will be ignored if so.


int OverlayImage_FindSpriteByID(OverlayImage *this, int spriteRegID,
		int *index);
// Purpose	:	Finds the specified sprite if it exist puts it's index into
// 				index.
// Pre		:	OverlayImage is initialized.
// Post		:	0 is returned if the sprite is not found, 
// 				1 is returned if the sprite was found, and the index
// 				is copied into index.
// Args		:	index: Can be NULL and will be ignored if so.
//


int OverlayImage_ModifySpriteOffset(OverlayImage *this, Sprite *sprite,
		int offsetX, int offsetY, int spriteRegID);
// Purpose	:	Sets the sprite's offset within the element and makes changes
// 				to it's boundingBox to keep it contained within the element.
// 				If sprite !=  NULL, then it is used to find the sprite,
// 				if sprite == NULL,  then the spriteRegID is used to find the 
// 				sprite.
// Pre		:	OverlayImage is initialized.
// Post		:	The sprite's offset is set within the element and it's
// 				boundingBox is modified to keep it within the element.
// Returns	:	 -1 if the sprite doesn't exist, 0 if it does.
// 
// Args		:	sprite can be NULL if a regID is provided, or sprite can be 
// 				the pointer to the sprite, and in that case the ID can be -1

#endif

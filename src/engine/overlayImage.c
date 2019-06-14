#include "engine/overlayImage.h"
#include "engine/spriteKaliDB.h"
// Private function declarations:
static void _OverlayImage_Destroy(void *thisPtr); // Destructor

static void _OverlayImage_Update(void *thisPtr, int offsetX, int offsetY);
// Purpose	:	Updates the position of all sprites in the element

static void _OverlayImage_SetVisibility(void *thisPtr,
		int visibility);

static void _OverlayImage_UpdateSpriteVisibility(OverlayImage *this, 
		int visibility);
// Purpose	:	Sets all sprites visibilities in the element





// Function Definitions

void OverlayImage_Init(OverlayImage *this, int width, int height, 
		int initialMaxSprites) {
	OverlayElement_Init(&this->_base, width, height);
	this->_images = malloc(sizeof(_OverlayImage_ImagePart) * initialMaxSprites);
	this->_numImageParts = 0;
	this->_sizeOfImagePartArray = initialMaxSprites;

	((OverlayElement *) this)->_Destructor = _OverlayImage_Destroy;
	((OverlayElement *) this)->_Update = _OverlayImage_Update;
	((OverlayElement *) this)->_SetVisibility = _OverlayImage_SetVisibility;
} // End OverlayImage_Init




void OverlayImage_Destroy(OverlayImage *this) {
	OverlayElement_Destroy(&this->_base);

	int i;
	for(i = 0; i < this->_numImageParts; i++) {	
		SpriteKaliDB_DestroySprite(this->_images[i]._sprite);
		this->_images[i]._sprite = NULL;
	}
	free(this->_images);
		
} // End OverlayImage_Destroy


int OverlayImage_AddSprite(OverlayImage *this, Sprite *sprite, 
		int offsetX, int offsetY) {
	int spriteExists = 0;
	int returnVal = 0;
	_OverlayImage_ImagePart *imagePart;

	if(this->_numImageParts == this->_sizeOfImagePartArray) {
		// Array full, must expand.
		this->_sizeOfImagePartArray *= 2;
		this->_images = realloc(this->_images, this->_sizeOfImagePartArray);
	}
		

	spriteExists = OverlayImage_FindSprite(this, sprite, NULL);

	if(!spriteExists) {
		// The sprite wasn't found so it can be added.
		imagePart = &this->_images[this->_numImageParts];
		imagePart->_sprite = sprite;
		imagePart->_offsetX = offsetX;
		imagePart->_offsetY = offsetY;
		this->_numImageParts++;
	}
	else {
		returnVal = -1;
	}
	
	return returnVal;
		

} // End OverlayImage_AddSprite




int OverlayImage_RemoveSprite(OverlayImage *this, Sprite *sprite) {
	int spriteExists = 0;
	int returnVal = 0;
	int numImageParts = this->_numImageParts;

	int indexOfSprite;

	if(OverlayImage_FindSprite(this, sprite, &indexOfSprite)) {
		// The sprite was found.
		spriteExists = 1;
		returnVal = 0;

		// Replace the to be removed sprite with the last sprite in the
		// element. 
		this->_images[indexOfSprite] = this->_images[numImageParts - 1];
		this->_numImageParts--;
	}

	if(!spriteExists) {
		returnVal = -1;
	}

	return returnVal;
	
} // End OverlayImage_RemoveSprite	




int OverlayImage_FindSprite(OverlayImage *this, Sprite *sprite, 
		int *index) {
	int numImageParts = this->_numImageParts;
	int i;
	int spriteExists = 0;

	for(i = 0; i < numImageParts; i++) {
		if(sprite == this->_images[i]._sprite) {
			if(index != NULL) {
				*index = i;
			}
			spriteExists = 1;
		}
	}

	if(!spriteExists) {
		if(index != NULL) {
			*index = -1;
		}
	}

	return spriteExists;

} // End OverlayImage_FindSprite




int OverlayImage_FindSpriteByID(OverlayImage *this, int spriteRegID,
		int *index) {
	int numImageParts = this->_numImageParts;
	int i;
	int spriteExists = 0;

	for(i = 0; i < numImageParts; i++) {
		if(spriteRegID == this->_images[i]._sprite->_regIndex) {
			if(index != NULL) {
				*index = i;
			}
			spriteExists = 1;
		}
	}

	if(!spriteExists) {
		if(index != NULL) {
			*index = -1;
		}
	}

	return spriteExists;
} // End OverlayImage_FindSpriteByID





int OverlayImage_ModifySpriteOffset(OverlayImage *this, Sprite *sprite,
		int offsetX, int offsetY, int spriteRegID) {
	int spriteExists = 0;
	int returnVal = 0;
	int indexOfSprite;

	if(sprite != NULL) {
		spriteExists = OverlayImage_FindSprite(this, sprite, &indexOfSprite);
	}
	else {
		// sprite was null, so the regID is used instead.
		spriteExists = OverlayImage_FindSpriteByID(this, spriteRegID, 
				&indexOfSprite);
	}



	if(spriteExists) {
		this->_images[indexOfSprite]._offsetX = offsetX;
		this->_images[indexOfSprite]._offsetY = offsetY;
	}
	else {
		returnVal = -1;
	}

	return returnVal;

} // End OverlayImage_ModifySpriteOffset











// static function definitions

void _OverlayImage_Destroy(void *thisPtr) {
	OverlayImage_Destroy(thisPtr);
} // End OverlayImage_Destroy


void _OverlayImage_Update(void *thisPtr, int offsetX, int offsetY) {
	int i;
	OverlayImage *this = thisPtr;
	int numSprites = this->_numImageParts;
	SDL_Rect *pos;

	for(i = 0; i < numSprites; i++) {
		// For each sprite, add the camera offset and the elements offset from
		// that offset, and add the sprites offset within the element.
		pos = &this->_images[i]._sprite->_boundingBox;
		pos->x = this->_images[i]._offsetX + this->_base._offsetX + offsetX;
		pos->y = this->_images[i]._offsetY + this->_base._offsetY + offsetY;
	}

	if(OverlayElement_IsVisible(thisPtr)) {
		_OverlayImage_UpdateSpriteVisibility(thisPtr, 1);	
	}
	else {
		_OverlayImage_UpdateSpriteVisibility(thisPtr, 0);	
	}


} // End _OverlayImage_Update


void _OverlayImage_SetVisibility(void *thisPtr, int visibility) {
	((OverlayElement *) thisPtr)->_isVisible = visibility;
} // End _OverlayImage_UpdateSpriteVisiblity




void _OverlayImage_UpdateSpriteVisibility(OverlayImage *this, int visibility) {
	int i;
	int numSprites = this->_numImageParts;

	for(i = 0; i < numSprites; i++) {
		Sprite_SetVisibility(this->_images[i]._sprite, visibility);
	}
} // End _OverlayImage_UpdateSpriteVisibility


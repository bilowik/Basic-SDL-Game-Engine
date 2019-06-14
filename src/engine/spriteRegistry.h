#ifndef SMPENGINE_SPRITEREGISTRY_H
#define SMPENGINE_SPRITEREGISTRY_H

#include "vector/vector.h"
#include "engine/sprite.h"
#include "engine/graphics.h"
#include "mapi/mapi.h"

typedef struct SpriteRegistry SpriteRegistry;
typedef struct Sprite Sprite;
typedef struct SpriteKaliDB SpriteKaliDB;


struct SpriteRegistry {
	Sprite **_registry;

	int *_emptyIndexStack;
	// Contains unused indices that are popped off when used to register a
	// sprite and then pushed back on when the sprite is unregistered.	

	int _numRegSprites;
	// Used to keep track of where the top of the index stack is.
	
	int _registrySize;
	// The size of the registry (the number of sprites it can contain).

	int _registryID;
	// A unique integer that represents this registry.
	// Common Use: A layer indicator Ex: Layer 3 = ID 3
	
};





void SpriteRegistry_Init(SpriteRegistry *this, int initialSize, int regID);
void SpriteRegistry_Destroy(SpriteRegistry *this);

void SpriteRegistry_Reg(SpriteRegistry *this, Sprite *sprite);
// Purpose	:	Adds a specific sprite into the sprite registry. Specifically,
// 				it's pointer.
// Pre		:	SpriteRegistry was initialized.
// Post		:	Sprite * is now registered in the registry.

void SpriteRegistry_Unreg(SpriteRegistry *this, int regID);
// Purpose	:	Remove a sprite from the sprite registry.
// Pre		:	SpriteRegistry is initialized.
// Post		:	The sprite was removed from the registry, but still may exist
// 				in the program. Must be destroyed if not to be used further.

int SpriteRegistry_ResizeRegistry(SpriteRegistry *this, int newSize);
// Purpose	:	Resizes the registry to the newSize
// Pre		:	SpriteRegistry is initialized
// Post		:	If newSize is not less than the current number of registered
// 					sprites, then the registry is resized to newSize
// 				else nothing is done.
// Return	:	-1 newSize is less than the number of registered sprites
// 				0 if the registry was resized.


void SpriteRegistry_DrawAll(SpriteRegistry *this, Graphics *graphics, 
		SDL_Rect *offset);
// Purpose	:	Draws all sprites in the sprite registry.
// 				(if they aren't set to invisible of course).
// Pre		:	Sprite registry has been intitialized
// Post		:	Sprite_Draw is called on every sprite.

void SpriteRegistry_UpdateAll(SpriteRegistry *this, uint32_t passedTicks, 
		void *instance);
// Purpose	:	Calls all registered sprites' Update method.
// Pre		:	Sprite registry is initialized.
// Post		:	All registered sprites have had their update function called.


const Sprite ** SpriteRegistry_GetPtrToIndex(SpriteRegistry *this, int regID);
// Purpose	:	Returns a pointer to the sprite pointer at the given index.
// 				Allows for checking if the sprite has been unregistered and
// 				destroyed, or if a sprite exists at this index.
// Pre		:	SpriteRegistry is initialized.
// Post		:	The pointer is returned for the given index.
// 				DO NOT WRITE TO THIS POINTER, READ ONLY


#endif

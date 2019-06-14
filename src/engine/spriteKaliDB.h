// Holds a database of sprite constructor and destructors that are linked
// to a specific ID.
//
// Note: Kali is the goddess of creation and destruction, hence the name,
// SpriteKali. More graceful and shorter than InitAndDestroyFuncs, Structors, 
// etc.


#ifndef SMPENGINE_SPRITE_KALI_DB_H
#define SMPENGINE_SPRITE_KALI_DB_H

#include "engine/graphics.h"
#include "mapi/mapi.h"
#include "engine/sprite.h"
#include "engine/spriteRegistry.h"



void SpriteKaliDB_Init(int initialSize);

void SpriteKaliDB_Destroy();


int SpriteKaliDB_AddSpriteKaliToDB(int spriteID,int sizeOfSprite, 
		void (*Constructor)(void *sprite, SpriteRegistry* reg,
			int id, Graphics *graphics),
		void (*Destructor)(void *sprite));
// Prupose	:	Adds a sprite constructor to the sprite
// 				registry.
// Pre		:	SpriteRegistry is initialized
// Post		:	The sprite constructor and destructor are added to the mapi,
// 				which maps the spriteID to the structors
// Return	:	0 if successful
// 				-1 if the id already existed.

Sprite *SpriteKaliDB_BuildSpriteByID(SpriteRegistry *reg, int spriteID,
		Graphics *graphics);
// Purpose	:	Returns a malloc'd pointer containing the sprite initialized
// 				using the given ID.
// Pre		:	SpriteRegistry is initialized.
// Post		:	*See Purpose*
// Return	:	NULL if no proper constructor has the specified ID
// 				Pointer to a malloc'd sprite if the id was valid.

int SpriteKaliDB_DestroySprite(Sprite *sprite);
// Purpose	:	Calls the destructor for the given sprite using it's ID
// Pre		:	SpriteRegistry is initialized.
// Post		:	The sprite's descrutor is called, and the proper memory is 
// 				freed.
// Return	:	-1 if the given sprite's ID was not valid and no destructor
// 					could be found
// 				0 if the given sprite's ID was found and it's destructor was
// 					called.
#endif

#ifndef SMPENGINE_WORLD_H
#define SMPENGINE_WORLD_H
#include <stdint.h>


typedef struct World World;
typedef struct WorldSection WorldSection;

// Forward declaration
typedef struct Sprite Sprite;
typedef struct SpriteRegistry SpriteRegistry;
typedef struct SDL_Rect SDL_Rect;


struct World {
	int xBoundary; // The "width" of the world
	int yBoundary; // The "height" of the world

	int sectionSize; // The length of the square section, in px
	int xNumSections;// The number of sections spaning the x-axis
	int yNumSections;// The number of sections spanning the y-axis


	// Whether the world border is a physical barrier or not.
	uint8_t topIsBarrier : 1;
	uint8_t rightIsBarrier : 1;
	uint8_t bottomIsBarrier : 1;
	uint8_t leftIsBarrier : 1;
	
	WorldSection **sections; // An array of arrays of WorldSections
	WorldSection **modifiedSectionStack;
	int numModifiedSections;

};


struct WorldSection {
	
	// The indices into the 2-D array where this section exists.
	int xSectionCoord;
	int ySectionCoord;
	
	
	Sprite **sprites; // The sprite that exist within this section.
	int sizeOfSpriteArr;
	int numSprites;


	uint8_t isModified : 1;
	// 1 if the section has been modified (as in, sprites are inside the section
	// 0 if the section has yet to be checked for sprites or sprites don't exist
	// in this section

};




void World_Init(World *this, int xBoundary, int yBoundary, int sectionSize,
		int initialSpriteCapOfSections);


void World_Destroy(World *this);


void World_CheckForCollisions(World *this);
// Purpose	:	Handles all collisions in the given World.
// Pre		:	World is initialized and reg is initialized.
// Post		:	For all found collisions, Sprite_CollisionHandler is called
// 				on the two colliding sprites.

int World_DidTheyCollide(SDL_Rect *rect1, SDL_Rect *rect2);
// Purpose	:	Checks if the two rectangles overlap on

void World_UpdateSections(World *this, Sprite *sprite);
// Purpose	:	Updates the sections array based on the given dimensions.
// Pre		:	World is initialized.
// Post		:	sections is initially cleared, and then the sections are 
// 				calculatd by the dimensions and added in.

void World_AddSpriteToSection(World *this, int xSectionInd, int ySectionInd,
		Sprite *sprite);
// Purpose	:	Adds the specified sprite into the section at the index
// 				of xSectionInd and ySectionINd
// Pre		:	WOrld is initialied.
// Post		:	The sprite has been added to the section specified.
// 				if the section was full, it was expanded.
// 				numSprites is incremented.

void World_RemoveSpriteFromSection(World *this, int xSectionInd, 
		int ySectionInd, Sprite *sprite);
// Purpose	:	Removes the specified sprite from the section
// Pre		:	World is initialized.
// Post		:	The sprite is removed from the section.
// 				nuMSprites is decremented.



void WorldSection_RemoveSprite(WorldSection *this, Sprite *sprite);
// Purpose	:	Remove a sprite from a specific section. 
// 				(See Above)


void World_CheckBounds(World *this, Sprite *sprite);
// Purpose	:	Checks the bounds of the sprite and adjusts them if the sprite
// 				is outside the bounds of the world. Acts like a barrier.
// Pre		:	World is initialized. Sprite is initialized.
// Post		:	The sprite now exists completely within the world


void World_GetSpritesAtCoords(World *this, int x, int y, Sprite **buffer, 
		int *numSprites);
// Purpose	:	Fills the array of sprite pointers with pointers to the sprites
// 				under the specified coords and increments numSprites with every
// 				sprite found.
// 				sprites that exist at the given x and y coordinate.
// Pre		:	World is initialized.
// Post		:	numSprites contains the number of sprites that existed at that
// 				coordinate
// 				A malloc'd array of sprite pointers is returned.
#endif

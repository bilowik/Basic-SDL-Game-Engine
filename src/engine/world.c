#include "engine/world.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "engine/sprite.h"

// Forward function declarations
void Sprite_ClearSections(Sprite *this);
void Sprite_AddSection(Sprite *this, WorldSection *section);
void Sprite_CollisionHandler(Sprite *this, Sprite *other);
int Sprite_GetIsStatic(Sprite *this);


void World_Init(World *this, int xBoundary, int yBoundary, int sectionSize,
		int initialSpriteCapOfSections) {
	int i, j;	
	
	this->xBoundary = xBoundary;
	this->yBoundary = yBoundary;	
	this->sectionSize = sectionSize;

	// Calculate the number of sections in the world.
	this->xNumSections = xBoundary / sectionSize;
	this->yNumSections = yBoundary / sectionSize;


	// Set the world barriers
	this->topIsBarrier = 1;
	this->rightIsBarrier = 1;
	this->bottomIsBarrier = 1;
	this->leftIsBarrier = 1;


	// Allocate memory for the sections
	this->sections = malloc(sizeof(WorldSection*) * this->xNumSections);
	for(i = 0; i < this->xNumSections; i++) {
		
		this->sections[i] = malloc(sizeof(WorldSection) * 
				this->yNumSections);
	

		for(j = 0; j < this->yNumSections; j++) {
			this->sections[i][j].xSectionCoord = i;
			this->sections[i][j].ySectionCoord = j;
			this->sections[i][j].sprites = malloc(sizeof(Sprite *) *
					initialSpriteCapOfSections);
			this->sections[i][j].sizeOfSpriteArr = 
				initialSpriteCapOfSections;
			this->sections[i][j].numSprites = 0;
			this->sections[i][j].isModified = 0;
		}
	}

	this->modifiedSectionStack = malloc(sizeof(WorldSection *) * 
			this->xNumSections * this->yNumSections);
	this->numModifiedSections = 0;

} // End World_Init




void World_Destroy(World *this) {
	int i,j;

	for(i = 0; i < this->xNumSections; i++) {
		for(j = 0; j < this->yNumSections; j++) {
			free(this->sections[i][j].sprites);
		}

		free(this->sections[i]);

	}

	free(this->sections);
	free(this->modifiedSectionStack);
	this->sections = NULL;


} // End World_Destroy

	

void World_CheckForCollisions(World *this) {

	WorldSection *currSection;
	Sprite **sectionSprites;
	Sprite *currSprite;
	Sprite *otherSprite;
	int numSprites;
	int i, j;

	while(this->numModifiedSections > 0 ) {
		currSection = this->modifiedSectionStack[--this->numModifiedSections];
		
		// Since after this processing this section will have been updated,
		// it is no longer modified.
		currSection->isModified = 0;

		if(currSection->numSprites < 2) {
			// There's either 1 or 0 sprites in this section, so no collisions
			// can occur, so skip this section entirely.
		
			continue; // Aware this is pointless but it's here for clarity.
		}
		else {
			// There's at least two sprites in this section, so collisions are
			// possible
			
			numSprites = currSection->numSprites;
			sectionSprites = currSection->sprites;
			for(i = 0; i < numSprites - 1; i++) {
				currSprite = sectionSprites[i];

				// The reason this loop is split between if statements
				// is because rather than check the same if statement every 
				// loop, it is done only once to further optimze this process.
				if(!Sprite_GetNoCollide(currSprite)) {
				if(Sprite_GetIsStatic(currSprite)) {
					for(j = i + 1; j < numSprites; j++) {
						otherSprite = sectionSprites[j];

						if(Sprite_GetIsStatic(otherSprite)) {
							// Both sprites are non-moving, so they cannot 
							// possibly collide
							continue;
						}
						else {
							// The other sprite is non-static so it is possible
							// that they could collide.
							if(!Sprite_GetNoCollide(otherSprite)) {
								// Both of the sprites can collide
								if(World_DidTheyCollide(
											&currSprite->_boundingBox,
											&otherSprite->_boundingBox)) {
									Sprite_CollisionHandler(currSprite, 
											otherSprite);
								}
							}

						}

					}

				}
				else { 
					// The sprite isn't static

					for(j = i + 1; j < numSprites; j++) {
						otherSprite = sectionSprites[j];
						if(!Sprite_GetNoCollide(otherSprite)) {	
							if(World_DidTheyCollide(
										&currSprite->_boundingBox,
										&otherSprite->_boundingBox)) {
								// They collided.
								Sprite_CollisionHandler(currSprite, otherSprite);
							}
						}
					}
				}
			} // End for(i = 0;...)
		}
		}

	} // End while(modifiedSectisons > 0)


} // End World_CheckForCollisions

			
			


int World_DidTheyCollide(SDL_Rect *rect1, SDL_Rect *rect2) {
	int withinXRange = 0;
	int withinYRange = 0;

	// Check if the sprites are within X's of eachother
	if(rect1->x >= rect2->x) {
		if(rect1->x <= (rect2->x + rect2->w)) {
			withinXRange = 1;
		}
	}
	else /*rect2->x < rect1->x*/ {
		if(rect2->x <= (rect1->x + rect1->w)) {
			withinXRange = 1;
		}
	}

	// Check if the sprites are within Y's of eachother
	if(rect1->y >= rect2->y) {
		if(rect1->y <= (rect2->y + rect2->h)) {
			withinYRange = 1;
		}
	}
	else /*rect2->y < rect1->y*/ {
		if(rect2->y <= (rect1->y + rect1->h)) {
			withinYRange = 1;
		}
	}


	return (withinXRange && withinYRange);

}





void World_UpdateSections(World *this, Sprite *sprite) {
	int i, j, sectionCountXDir, sectionCountYDir;
	int xSection;
	int ySection;
	int sectionSize = this->sectionSize;
	WorldSection **sections = this->sections;
	WorldSection *currXSection;
	WorldSection *currSection;
	SDL_Rect *box = &sprite->_boundingBox;

	// Check and move the sprite if it's crossed outside this world's boundary
	World_CheckBounds(this, sprite);

	// Clear the sprites current sections
	Sprite_ClearSections(sprite);

	// Get beginning section	
	xSection = box->x / sectionSize;
	ySection = box->y / sectionSize;	
	
	// Calculate how many sections the this spans in both directions:
	sectionCountXDir = ((box->x + box->w - 1) / 
			sectionSize) - xSection + 1;

	sectionCountYDir = ((box->y + box->h - 1) / 
			sectionSize) - ySection + 1; 

	 

	// Now iterate through, starting with the top left section, to the 
	// bottom right section and add the sprite to those sections if it 
	// doesn't already exist.
	for(i = 0; i < sectionCountXDir; i++) {
		currXSection = sections[xSection];
			
		for(j = 0; j < sectionCountYDir; j++) {
		
			/*
			if(ySection >= this->yNumSections) {
				// This usually happens when a sprite is AT the edge of the
				// world b
				continue;
			}
			*/
	

			currSection = &currXSection[ySection];	
		
			

			// Add the sprite to the section and the section to the sprite:	
			Sprite_AddSection(sprite, currSection);		
			World_AddSpriteToSection(this, xSection, ySection, sprite);

			
			if(!currSection->isModified) {
				this->modifiedSectionStack[this->numModifiedSections++] =
					currSection;
				currSection->isModified = 1;
			}
			
				

			ySection++; // Increment the sprite->_boundingBox.y index
		
		} // End for(j=...)
			
		xSection++; // Increment the sprite->_boundingBox.x index
		ySection = box->y / sectionSize; // Reset ySection
	} // End for(i=...)

	


} // End Sprite_UpdateSections




void World_AddSpriteToSection(World *this, int xSectionInd, int ySectionInd,
		Sprite *sprite) {
	WorldSection *section = &this->sections[xSectionInd][ySectionInd];
	if(section->numSprites == section->sizeOfSpriteArr) {
		// Must be resized.
		section->sizeOfSpriteArr *= 2;
		section->sprites = realloc(section->sprites, 
				section->sizeOfSpriteArr * sizeof(Sprite *));
			
	}

	section->sprites[section->numSprites] = sprite;
	section->numSprites++;

	// Add section to modifiedSectionStack
	if(!section->isModified) {
		this->modifiedSectionStack[this->numModifiedSections++] = section;
		section->isModified = 1;
	}

} // End World_AddSpriteToSection




void World_RemoveSpriteFromSection(World *this, int xSectionInd, 
		int ySectionInd, Sprite *sprite) {
	WorldSection *section = &this->sections[xSectionInd][ySectionInd];
	int i;
	int spriteIndex = -1;
	for(i = 0; i < section->numSprites; i++) {
		if(sprite == section->sprites[i]) {
			spriteIndex = i;
			break;
		}
	}
	
	if(spriteIndex != -1) {
		// Replace the removed sprite with the sprite at the end of the array:
		section->sprites[spriteIndex] = 
			section->sprites[section->numSprites - 1];
		section->numSprites--;
	}


} // End World_RemoveSpriteFromSection




void WorldSection_RemoveSprite(WorldSection *this, Sprite *sprite) {
	int i;
	int spriteIndex = -1;
	for(i = 0; i < this->numSprites; i++) {
		if(sprite == this->sprites[i]) {
			spriteIndex = i;
			break;
		}
	}
	
	if(spriteIndex != -1) {
		// Replace the removed sprite with the sprite at the end of the array:
		this->sprites[spriteIndex] = 
			this->sprites[this->numSprites - 1];
		this->numSprites--;
	}

} // End WorldSection_RemoveSprite





void World_CheckBounds(World *this, Sprite *sprite) {	
	SDL_Rect *boundingBox = Sprite_GetBoundingBoxPtr(sprite);	
	SDL_Rect newBox = *boundingBox;	
	if(boundingBox->x < 0) {
		newBox.x = 0;
		Sprite_SetBoundingBox(sprite, newBox);
	}
	if(boundingBox->y < 0) {	
		newBox.y = 0;
		Sprite_SetBoundingBox(sprite, newBox);
	}
	if((boundingBox->x + boundingBox->w) > this->xBoundary) {
		newBox.x = this->xBoundary - boundingBox->w;
		Sprite_SetBoundingBox(sprite, newBox);
	}
	if((boundingBox->y + boundingBox->h) > this->yBoundary) {
		newBox.y = this->yBoundary - boundingBox->h;
		Sprite_SetBoundingBox(sprite, newBox);
	}

	// NOTE: The reason SetBoundingBox is in all the if statmeents and not
	// at the end of the function is because it's a rare case that the sprite
	// should ever touch the bounds, so to call a function and pass by copy
	// the entire boundingBox every single call for every sprite, it will only
	// do so when it does touches the bounds.
} // End World_CheckBounds




void World_GetSpritesAtCoords(World *this, int x, int y, Sprite **buffer, 
		int *numSprites) {
	*numSprites = 0;
	Sprite *currSprite;

	int xSection = x / this->sectionSize;
	int ySection = y / this->sectionSize;
	

	WorldSection *currSection = &this->sections[xSection][ySection];

	int i;
	int numSpritesInSection = currSection->numSprites;
	for(i = 0; i < numSpritesInSection; i++) {
		currSprite = currSection->sprites[i];
		
		if(Sprite_ExistsAtCoords(currSprite, x, y)) {
			// Sprite overlaps the given coords
			buffer[*numSprites] = currSprite;
			(*numSprites)++;
		}
	}

} // End World_GetSpritesAtCoords



		
	
	



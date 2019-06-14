#ifndef SMPENG_SPRITE_H
#define SMPENG_SPRITE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include "map/map.h"
#include "engine/basicPhys.h"

#define MAX_SPRITES_PER_SHEET 1024
#define INITIAL_TAGS_MAP_SIZE 10

// Forward Declarations
typedef struct SpriteRegistry SpriteRegistry;
typedef struct Map Map;
typedef struct World World;
typedef struct WorldSection WorldSection;
typedef struct Graphics Graphics;
typedef struct BasicWorldPhysProps BasicWorldPhysProps;


typedef struct Sprite Sprite;
typedef struct UpdateFuncNode UpdateFuncNode;
typedef struct Tag Tag;


//extern float G_SPRITE_DRAW_DIM_MULT_W;
//extern float G_SPRITE_DRAW_DIM_MULT_H;



struct Sprite {
	char* _path;
	char* _name;
	SDL_Texture* _sheet;
	SDL_Rect* _partRectArray;
    // Array of Rects that denote different sections
	// the Sprite.
	
	char** _partNames;    
    // Array of strings that point to the names of the individual Sprite
	// parts.
		
	uint16_t _numParts;
	// Total number of Sprite parts in the sheet
	

	uint16_t _currPart;
	// The index into '_partRectArray' that points to the current sprite
	// to be used.
	

	SDL_Rect _boundingBox;
	// The position of the sprite and it's size on the renderer

	float _rotation;
	// The rotation of the sprite

	SDL_Point _rotationPoint;
	// The Y coordinate relative to the top left of the sprite to be the center
	// of rotation
	// Ignored if rotationCentered is 1
	

	int _regIndex;
	// Will always be the index where the sprite is located in the
	// registry. 
	
	SpriteRegistry *_reg;
	// A pointer to the registry this sprite belongs to.
	
	BasicPhysProps _props;
	// Basic physical properties for sprites. 


	int _spriteID;
	// The ID of the type of sprite.
	
	uint16_t _isVisible : 1;
	// = 0 if invisible
	// = 1 if visible
	
	uint16_t _isClickable : 1;
	// = 0 if it is not clickable
	// = 1 if it is clickable
	
	uint16_t _noCollide : 1;
	// = 1 if sprite does not collide with anything.
	// = 0 if it does collide with at least one thing. 

	uint16_t _ignoreOffset : 1;
	// = 1 if it will ignore a drawing offset
	// = 0 if it will not ignoe a drawing offset

	uint16_t _isStatic : 1;
	// = 1 if the sprite's _boundingBox will remain constant.
	// = 0 if the sprite's _boundingBox is subject  to change.
	// For any sprite that is set to be static, Sprite_UpdateSections must be
	// called at least ONCE so that sprite can be set into the proper section.

	uint16_t _wasManuallyUpdated : 1;
	// == 1 if the sprite has already been updated 
	// == 0 if the sprite has yet to be updated.
	// Main use: When you want certain sprites to be updated before others,
	// or in a specific order, you can call Sprite_ManualUpdate on those 
	// specific sprites prior to calling SpriteRegistry_UpdateAll. This tells 
	// that function to skip updating this sprite because it's already been
	// updated.
	
	uint16_t _isRotated : 1;
	// If this is 1, a rotation check is made and a rotation is made upon
	// rendering the sprite
	// If 0, then all rotation information is ignored
	uint16_t _rotationCentered : 1;
	// If this is 1, then rotation will always be centered and the x and y
	// rotation points are ignored.
	// IF this is 0, then the rotation points are used instead.

	uint16_t _flipped : 2;
	// Determines whether the image should be flipped when rendered.
	// SDL_RENDER_NONE for no flip
	// SDL_RENDER_HORIZONTAL for horizontal flip
	// SDL_RENDER_VERTICAL for vertical fip
	// Default is SDL_RENDER_NONE

	uint32_t _timeAlive;
	// The time that the sprite has existed for.


	Map _tags;
	// Contains tags (strings) that are paired with a value.
	// EX: A "TimeTillDeath" tag can be paired with uint32_t value, and upon
	// _timeAlive reacing that value the sprite can self-destruct.

	WorldSection **_sections;
	int _numSections;
	int _sizeOfSectionArr;
	// Contains an array of section pointers that point to sections that
	// this sprite currently exists in. 
	
	
	void (*_onLeftClick)(Sprite* this, int mouseX, int mouseY, 
				void* additionalArgs);
	// Purpose	:	Called when the mouse left clicks the sprite
	// Pre		:	'Sprite' has been initialized. 'OnLeftClick'
	// 			points to a valid function
	// Post		:	Some action is performed (or none) 
	// void* additionalArgs :	Can be used to send additional
	// 				arguments
	
	void (*_onRightClick)(Sprite* this, int mouseX, int mouseY, 
				void* additionalArgs);
	// Purpose	:	Called when the mouse right clicks the sprite
	// Pre		:	'Sprite' has been initialized. 'OnRightClick'
	// 			points to a valid function
	// Post		:	Some action is performed (or none) 
	// void* additionalArgs :	Can be used to send additional
	// 				arguments
	

	void (*OnCollision)(void *thisSprite, void *otherSprite);
	// Collision handler. This function will perform additional actions 
	// not relating to basic physics calcuations, which will be performed
	// only once by Sprite_CollisionHandler

	UpdateFuncNode *_UpdateFunctions;
	// Purpose	:	Provide methods that are attached to all Sprites and structs
	// 				that inherit it, that will serve to update the contents of 
	// 				the struct.
	// Pre		:	'Sprite' has been initialized, as well as it's inheritee if
	// 				it is one.
	// Post		:	Updates propterties of the sprite.

};


struct UpdateFuncNode {
	UpdateFuncNode *next;
	void (*Update)(void *this, uint32_t passedTicks, void *args);
};

struct Tag {
	char *name;
	void *data;
	int sizeOfData;
};


void Sprite_Init(void *thisPtr, SpriteRegistry *registry, const char* filePath, 
		Graphics* graphics);

void Sprite_Destroy(void *thisPtr);


void Sprite_Update(Sprite *this, uint32_t passedTicks, SpriteRegistry *reg, 
		void *instance);
// Purpose	:	Calls all of the sprite's update functions
// Pre		:	Sprite is initialized.
// Post		:	All of the sprite's update functions are called, unless one
// 				of those functions prematurally destroys the sprite, in 
// 				which case this function is exited and the rest of the 
// 				update functions are not called.


void Sprite_ManualUpdate(Sprite *this, uint32_t passedTicks,
		SpriteRegistry *reg, void *instance);
// To be called instead of Sprite_Update when updating a sprite outside
// of the sprite registry.

void Sprite_UpdatePhys(Sprite *this, BasicWorldPhysProps *worldProps, 
		uint32_t passedTicks);
// Purpose	:	Updates the sprite's physics properties and keeps it's 
// 				visual properties (boundingBox) in sync with them.
// Pre		:	Sprite is initialized.
// Post		:	Sprite's physical properties have been updated and its 
// 				visual properties synced with them. 



void Sprite_InitTextureDB(int initialSize);
// MUST be called before loading any textures.
// Initializes the Map struct that will save textures once they are loaded
// into memory so they don't have to be reloaded every time it is used
// in a sprite.

void Sprite_ClearTextureDB();
// Can be called to clear the texture DB. 

void Sprite_ResizeTexureDB(int newSize);
// Resizes the SpriteDB
// The SpriteDB is dynamic and will automatically resize itself, but
// for efficiency, you can resize it once if you know there's going to be
// a lot of new textures.




void Sprite_DestroySpriteRegistry();

void Sprite_Draw(Sprite* this, Graphics* graphics, SDL_Rect *offset);
	// Purpose	:	Draw Sprite part to the renderer
	// Pre		:	'Sprite' is initialized by a constructor
	// Post		:	The texture designated by
	// 			'this->_partRectArray[this->_currPart] is
	// 			drawn to the renderer using
	// 			'this->_boundingBox'

void Sprite_SetBoundingBox(Sprite* this, SDL_Rect newBox);
	// Purpose	:	Sets the position and size of the sprite on the
	// 			renderer. Updates 'props' with newBox's x, and y values.
	// Pre		:	'Sprite' has been initialized by a constructor
	// Post		:	'_boundingBox' is updated with the values of 
	// 			'newBox'. 'props' has been updated with the x and y values
	// 			from newBox.

void Sprite_SetDimensions(Sprite *this, int w, int h);
// Purpose	:	Sets the width and height of the sprite's bounding box.
// Pre		:	Sprite is initialized
// Post		:	Sprite's bounding box w and h are now set.

void Sprite_SetPos(Sprite *this, int x, int y);
// Purpose	:	Sets the x and y values of the sprites bounding box, as well
// 				as it's props x and y values as well.
// Pre		:	Sprite is initialized.
// Post		:	boundingBox.x and .y are set, and props.xPos and .yPos are
// 				set.

void Sprite_SetPrecisePos(Sprite *this, float x, float y);
// Purpose	:	Sets a more precise position for the sprite's position, 
// 				specifically for its basic props.
// Pre		:	Sprite is initialized.
// Post		:	boundingBox.x and .y are set, props.xPos and .yPos are set.

void Sprite_SetRotation(Sprite *this, float rotation);
// Sets _rotation 

void Sprite_SetIsRotated(Sprite *this, int isRotated);
// Sets _isRotated

void Sprite_SetRotationPoint(Sprite *this, int pointX, int pointY);
// Sets the x and y coordinate that the sprite will be rotated around.

void Sprite_SetRotationCentered(Sprite *this, int isCentered);
// Centers the rotation around the center point of the sprite

void Sprite_SetFlip(Sprite *this, int flip);
// Sets _flipped to flip, must be SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL,
// or SDL_FLIP_VERTICAL

void Sprite_GetPos(Sprite *this, int *x, int *y);
// Sets *x and *y to boundingBox.x and .y

void Sprite_GetPrecisePos(Sprite *this, float *x, float *y);
// Sets *x and *y to porps.posX and .posY

float Sprite_GetRotation(Sprite *this);
// Returns _rotation

int Sprite_GetIsRotated(Sprite *this);
// Returns _isRotated

void Sprite_GetRotationPoint(Sprite *this, int *pointX, int *pointY);
// pointX and pointY get the respective rotation points

int Sprite_GetRotationCentered(Sprite *this);
// Returns _rotationCentered

int Sprite_GetFlip(Sprite *this);
// Returns _flipped

void Sprite_GetSpritePartDimensions(Sprite* this, int* w, int* h);
// 'w' gets the width of the current sprite part
// 'h' gets the height of the current sprite part

SDL_Rect* Sprite_GetBoundingBoxPtr(Sprite* this);
// Returns a pointer to ths sprite's boundingBox

void Sprite_SetCurrPart(Sprite* this, int newPart);
// Purpose	:	Sets the current part of the sprite sheet.
// Pre		:	'Sprite' has been initialized by a constructor
// Post		:	'_currPart' is set to 'newPart'


void Sprite_SetVisibility(Sprite* this, uint8_t visibility);
// Purpose	:	Set the visibility of the sprite
// Pre		:	'Sprite' has been initialized.
// Post		:	'_isVisible' is set to 'visibility'

void Sprite_SetNoCollide(Sprite *this, uint8_t noCollide);
// Sets _isCollidable to the argument collidable.
// 0 = not collidable
// 1 = is collidable

int Sprite_GetNoCollide(Sprite *this);

void Sprite_SetClickable(Sprite *this, uint8_t clickable);
// 0 = not clickable
// 1 = clickable

void Sprite_SetIsStatic(Sprite *this, int isStatic);
int Sprite_GetIsStatic(Sprite *this);

void Sprite_AddUpdateFunc(Sprite *this, 
		void (*UpdateFunc)(void *this, uint32_t passedTicks, void *args));


int Sprite_AddTag(Sprite *this, const char *tagName, const void *tagData,
		int sizeOfTagData);
// Purpose	:	Adds a *NEW* tag to the sprite and copies the name and data
// 				into the tag. 
// Pre		:	Sprite is initialized
// Post		:	If a tag with the same name doesn't exist, then the tag is
// 				added, if it already exists, it is not added.
// Return	:	0 if the tag already didn't exist and was added.
//				-1 if the tag already exists and the tag wasn't added.

int Sprite_ModifyTag(Sprite *this, const char *tagName, const void *tagData);
// Purpose	:	Modifies an existing tag.
// Pre		:	Sprite is initialized
// Post		:	If the tag existed, then the data is copied over over the 
// 				preexisting data. If the tag doesn't exist, then nothing is
// 				done.
// Return 	:	0 if the tag already exists
// 				-1 if the tag didn't already exist.


int Sprite_RemoveTag(Sprite *this, const char *tagName);
// Purpose	:	Removes a tag and it's data.
// Pre		:	Sprite is initialized.
// Post		:	If the specified tag exists, it is removed, if it doesn't exist,
// 				nothing is done.
// Return	:	0 if the tag existed and was removed.
// 				-1 if the tag did not exist.


int Sprite_DoesTagExist(Sprite *this, const char *tagName);
// Purpose	:	Check if a tag exists
// Pre		:	Sprite is initialized.
// Post		:	Returns whether the tag exists or not.
//
// Return	:	1 if the tag exists
// 				0 if the tag doesn't exist.

int Sprite_GetTagData(Sprite *this, const char *tagName, void *dataBuf, 
		int sizeOfDataBuf, int *sizeOfData);
// Purpose	:	Gets the tag data from the specified tag.
// Pre		:	Sprite is initialized
// 				dataBuf is NOT NULL
// Post		:	If the tag eixsts
// 					if sizeOfDataBuf >= the size of the data, then the data 
// 					is copied into dataBuf. If it is not <, then no data is 
// 					copied. Regardless, sizeOfData gets the size of the data.
// 				Else, nothing is copied into dataBuf.
// 				If sizeOfData is NULL, then the size is not copied.
// Return	:	0 if the tag existed and the data was copied into dataBuf
//
// 				-1 if the tag existed, but dataBuf was not large enough to
// 				hold the data
//
// 				-2 if the tag didn't exist.

void *Sprite_GetTagDataPtr(Sprite *this, const char *tagName);
// Purpose	:	Returns a pointer to the data associated with the tagName.
// Pre		:	Sprite is initialized.
// Post		:	If the tag exists, a pointer to it's data is returned.
// 				else, NULL is returned.
// Return	:	NULL if tag doesn't exist
// 				Valid pointer if the tag exists.

void Sprite_UpdateSections(Sprite *this, World *world);
// Purpose	:	Updates where the sprite exists in the world.
// Pre		:	Sprite is initialized, World is initialized.
// Post		:	Sprite->_sections is updated, and expanded if needed.
// 				This sprite is added to the sections as well. 

void Sprite_PingSections(Sprite *this, World *world); 
// Purpose	:	Ping the current sections so the world knows that this sprite
// 				exists in them
// Pre		:	Sprite is initialized. World is initialized.
// Post		:	This sprite's sections are added on the stack of modified 
// 				sections. (active sections). 


void Sprite_ClearSections(Sprite *this);
// Purpose	:	Clears the sprites sections and removes the sprite from those
// 				sections.
// Pre		:	Sprite is initialized.
// Post		:	Sprite now exists in 0 sections and has been removed from all
// 				previous sections it existed in.

void Sprite_AddSection(Sprite *this, WorldSection *section);
// Purpose	:	Adds the specified section to the end of the sprtie's section
// 				array.
// Pre		:	Sprite is initialized, and it ClearCurrSections has been called
// 				since the last time this sprite had added this section.
// Post		:	section is added to the end of the section array, 
// 				numSections is incremented, and the section array is expanded
// 				if it was full. 

void Sprite_CollisionHandler(Sprite *this, Sprite *other);
// Purpose	:	Handles a collision between the two given sprites and also
// 				calls their specified collision handlers.
// Pre		:	Sprite's are initialized.
// Post		:	The sprites' collision handlers is called.
// 				Then For all sprites that are non-static and non-collide, a 
// 				physics calculation is made on their physics properties.

//void Sprite_SetWorldProps(Sprite *this, BasicWorldPhysProps *world);
// Copies the pointer to the world props to this sprites props



void Sprite_GetSidesOfCollision(Sprite *sprite1, Sprite *sprite2, 
		int *sideSprite1, int *sideSprite2);
// Purpose	:	Calculates the sides of the collision for both sprites
// Pre		:	Sprites are initialized, pointers are not NULL
// Post		:	*sideSprite1 and *sideSprite2 are set to the sides their
// 				respective sprite was collided on.
// 				0 = top
// 				1 = left
// 				2 = bottom
// 				3 = right

int Sprite_ExistsAtCoords(Sprite *this, int x, int y);
// Purpose	:	Checks if this sprite touches the coordinates given. (Overlaps
// 				the given coordinates)
// Pre		:	Sprite is initialized.
// Post		:	1 is returned if true, 0 if false.

// MACROS
// These function macros check if the current sprite is above/left/right/below
// the other given sprite.
#define Sprite_IsAbove(this, otherSprite) (this->_boundingBox.y < \
	otherSprite->_boundingBox.y)

#define Sprite_IsBelow(this, otherSprite) (this->_boundingBox.y > \
	otherSprite->_boundingBox.y)

#define Sprite_IsLeft(this, otherSprite) (this->_boundingBox.x < \
	otherSprite->_boundingBox.x)

#define Sprite_IsRight(this, otherSprite) (this->_boundingBox.x > \
	otherSprite->_boundingBox.x)




#endif


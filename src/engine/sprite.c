#include "engine/sprite.h"
#include "vector/vector.h"
#include "queue/queue_t/int/queue_t.h"
#include "engine/graphics.h"
#include "engine/world.h"
#include "engine/globals.h"
// Global definitions
//float G_SPRITE_DRAW_DIM_MULT_W = 1.0f;
//float G_SPRITE_DRAW_DIM_MULT_H = 1.0f;


static const char SMPENGINE_SPRITE_DATA_EXT[] = ".ssd";

static Map _loadedTextures;
// A Map that maps a string (the path to the image file) to an SDL_Surface
// This allows for multiple sprites to use a single loaded Texture to create
// their texture from. Saves memory.


// Private methods


static void _spriteDataParser(Sprite* this);
// Takes the original file path of the Sprite and looks for it's corresponding
// .ssd file that holds the information pertaining to the Sprite's sections

static void _parseSpriteNameFromPath(char* path, char* buffer);
// The sprite's name is prased from 'path' and placed in 'buffer'


// The default function to be pointed to by sprites' onClick methods.


// Forward declarations:
void SpriteRegistry_Reg(SpriteRegistry *this, Sprite *sprite);
void SpriteRegistry_Unreg(SpriteRegistry *this, int regID);
const Sprite ** SpriteRegistry_GetPtrToIndex(SpriteRegistry *this, int regID);
// Constructors



void Sprite_Init(void *thisPtr, SpriteRegistry *registry, const char* filePath, 
		Graphics* graphics) {
	Sprite *this = thisPtr;
	if(filePath != NULL) {
		if(Map_At(&_loadedTextures, filePath) != NULL) {
			// Texture is already loaded.
		   this->_sheet = *(SDL_Texture**) Map_At(&_loadedTextures, filePath);
		}
		else {
			// The textures hasn't been loaded
			SDL_Surface* tempSurface = IMG_Load(filePath);
			this->_sheet = SDL_CreateTextureFromSurface(graphics->_renderer, 
					tempSurface);
			if(this->_sheet == NULL) {
				// Error
				printf("ERROR: Sprite_Init failed. Error creating texture. %s\r\n",
						SDL_GetError());
			}
			Map_Set(&_loadedTextures, filePath, &this->_sheet); 

			SDL_FreeSurface(tempSurface);
		}
		this->_path = malloc(strlen(filePath) + 1);	
		strcpy(this->_path, filePath);
		_spriteDataParser(this);
		this->_boundingBox.w = this->_partRectArray[0].w;
		this->_boundingBox.h = this->_partRectArray[0].h;
	}
	else {	
		this->_partRectArray = malloc(sizeof(SDL_Rect));
		this->_partRectArray[0] = (SDL_Rect) {0,0,0,0};
	}

	this->_boundingBox.x = 0;
	this->_boundingBox.y = 0;
	this->_boundingBox.w = this->_partRectArray[0].w;
	this->_boundingBox.h = this->_partRectArray[0].h;
	this->_rotation = 0.0f;	
	
	this->_flipped = SDL_FLIP_NONE;
	
	this->_UpdateFunctions = NULL;

	this->_isVisible = 0;
	this->_isClickable = 0;
	this->_noCollide = 0;
	this->_ignoreOffset = 0;
	this->_currPart = 0;
	this->_timeAlive = 0;
	this->_isStatic = 0;
	this->_isRotated = 0;
	this->_wasManuallyUpdated = 0;


	// Register sprite
	this->_reg = registry;
	SpriteRegistry_Reg(registry, this);


	// Initialize tags
	Map_Init(&this->_tags, sizeof(Tag), INITIAL_TAGS_MAP_SIZE);

	
	// Initialize basic physical properties
	BasicPhysProps_Init(&this->_props);
	this->OnCollision = NULL;	


	// Initialize sections
	this->_sections = malloc(sizeof(WorldSection *) * 4);
	this->_numSections = 0;
	this->_sizeOfSectionArr = 4;


} // End init_sprite




void Sprite_Update(Sprite *this, uint32_t passedTicks, SpriteRegistry *reg, 
		void *args) {

	if(!this->_wasManuallyUpdated) {
		int regID = this->_regIndex;
		 Sprite const **spriteRegistryPtr = 
			 SpriteRegistry_GetPtrToIndex(reg, regID);
		this->_timeAlive += passedTicks;	
		UpdateFuncNode *currNode;
		currNode = this->_UpdateFunctions;
		if(currNode != NULL) {
			while(currNode->Update != NULL) {
				currNode->Update(this, passedTicks, args);
				
				if(*spriteRegistryPtr == NULL) {
					// Sprite has been destroyed and unregistered, must quit 
					// updating.
					break;	
				}

				else {
				currNode = currNode->next;
				}	
			}

		}
	}
	else {
		// This sprite was manually updated.
		// Set to 0 for the next round of updates
		this->_wasManuallyUpdated = 0;
	}
} // End Sprite_Update



void Sprite_ManualUpdate(Sprite *this, uint32_t passedTicks,
		SpriteRegistry *reg, void *instance) {
	Sprite_Update(this, passedTicks, reg, instance);
	this->_wasManuallyUpdated = 1;
}



void Sprite_UpdatePhys(Sprite *this, BasicWorldPhysProps *worldProps, 
		uint32_t passedTicks) {
	BasicPhys_UpdateObj(worldProps, &this->_props, passedTicks);
	this->_boundingBox.x = this->_props.posX;
	this->_boundingBox.y = this->_props.posY;
}



void Sprite_Destroy(void* thisPtr) {
	Sprite *this= thisPtr;
	// Remove from registry:
	SpriteRegistry_Unreg(this->_reg, this->_regIndex);


	// Destroy sections and remove this sprite from those sections
	Sprite_ClearSections(thisPtr);
	free(this->_sections);
	this->_sections = NULL;

	free(this->_name);
	this->_name = NULL;
	free(this->_path);
	this->_path = NULL;
	free(this->_partRectArray);
	this->_partRectArray = NULL;
	
	int i;
	for(i = 0; i < this->_numParts; i++) {
		free(this->_partNames[i]);
		this->_partNames[i] = NULL;
	}

	free(this->_partNames);

	// Clear the update functions
	UpdateFuncNode *currNode, *prevNode;
	currNode = this->_UpdateFunctions;
	prevNode = currNode;
	while(currNode != NULL) {
		prevNode = currNode;
		currNode = currNode->next;
		free(prevNode);
	}
	
	//SDL_DestroyTexture(this->_sheet);
	// TODO: Destroying this texture removes it for all other 
	// sprites that may be using that texture. Need to figure out a way
	// to gracefully destroy specific textures after the client is done
	// using them

	Map_Destroy(&this->_tags);
} // End Sprite_Destroy




void Sprite_InitTextureDB(int initialSize) {
	Map_Init(&_loadedTextures, sizeof(SDL_Texture*), initialSize);

} // End Sprite_InitTextureDB





void Sprite_ClearTextureDB() {
	int i;
	for(i = 0; i < _loadedTextures._numItems; i++) {
		SDL_DestroyTexture(_loadedTextures._array[i].data);
	}
	Map_Clear(&_loadedTextures);
} // End Sprite_ClearTextureDB




void Sprite_ResizeTextureDB(int newSize) {
	Map_Resize(&_loadedTextures, newSize);
} // End Sprite_ResizeTextureDB





// Public method implementations

void Sprite_Draw(Sprite* this, Graphics* graphics, SDL_Rect *offset) {
	SDL_Rect box;
	SDL_Point *rotationPoint;
	float rotation = 0.0f;	
	if(this->_currPart >= this->_numParts) {
		printf("ERROR: Sprite: %s does not contain a part with "
				"index %d %c", this->_name, this->_currPart,
				'\n');
	}
	else {

		if(this->_isVisible) {
			box = this->_boundingBox;
			box.x = box.x - offset->x;
			box.y = box.y - offset->y;


			if(this->_isRotated) {
				rotation = this->_rotation;
				if(this->_rotationCentered) {
					rotationPoint = NULL;
				}
				else {
					rotationPoint = &this->_rotationPoint;
				}
			}


			

			SDL_RenderCopyEx(graphics->_renderer, this->_sheet,
						&this->_partRectArray[this->_currPart], &box,
						rotation, rotationPoint, 
						this->_flipped);
		
		}
	}
} // End _draw



void Sprite_SetBoundingBox(Sprite* this, SDL_Rect newBox) {
	this->_boundingBox = newBox;
	this->_props.posX = newBox.x;
	this->_props.posY = newBox.y;
} // End _setBoundingBox



void Sprite_SetDimensions(Sprite *this, int w, int h) {
	this->_boundingBox.w = w;
	this->_boundingBox.h = h;
} // End Sprite_SetDimensions



void Sprite_SetPos(Sprite *this, int x, int y) {
	this->_boundingBox.x = x;
	this->_boundingBox.y = y;
	this->_props.posX = (float) x;
	this->_props.posY = (float) y;
} // End Sprite_SetPos



void Sprite_SetPrecisePos(Sprite *this, float x, float y) {
	this->_boundingBox.x = (int) x;
	this->_boundingBox.y = (int) y;
	this->_props.posX = x;
	this->_props.posY = y;
} // End Sprite_SetPrecisePos

void Sprite_SetRotation(Sprite *this, float rotation) {
	this->_rotation = rotation;
} // End Sprite_SetRotation

void Sprite_SetIsRotated(Sprite *this, int isRotated) {
	this->_isRotated = isRotated;
} // End Sprite_SetIsRotated

void Sprite_SetRotationPoint(Sprite *this, int pointX, int pointY) {
	this->_rotationPoint.x = pointX;
	this->_rotationPoint.y = pointY;
} // End Sprite_SetRotationPoints

void Sprite_SetRotationCentered(Sprite *this, int isCentered) {
	this->_rotationCentered = isCentered;
} // End Sprite_SetRotationCentered

void Sprite_SetFlip(Sprite *this, int flip) {
	if(flip <= 2) {
		this->_flipped = flip;
	}
} // End Sprite_SetFlip

void Sprite_GetPos(Sprite *this, int *x, int *y) {
	*x = this->_boundingBox.x;
	*y = this->_boundingBox.y;
} // End Sprite_GetPos

void Sprite_GetPrecisePos(Sprite *this, float *x, float *y) {
	*x = this->_props.posX;
	*y = this->_props.posY;
} // End Sprite_GetPrecisePos

float Sprite_GetRotation(Sprite *this) {
	return this->_rotation;
} // End Sprite_GetRotation

int Sprite_GetIsRotated(Sprite *this) {
	return this->_isRotated;
} // End Sprite_GetIsRotated

int Sprite_GetFlip(Sprite *this) {
	return this->_flipped;
} // End Sprite_GetFlip

void Sprite_GetSpritePartDimensions(Sprite* this, int* w, int* h) {
	*w = this->_partRectArray[this->_currPart].w;
	*h = this->_partRectArray[this->_currPart].h;
} // End _getSpritePartDimensions



SDL_Rect* Sprite_GetBoundingBoxPtr(Sprite* this) {
	return &this->_boundingBox;
} // End _getBoundingBox


void Sprite_SetCurrPart(Sprite* this, int newPart) {
	this->_currPart = newPart;
} // End _setCurrPart



void Sprite_SetVisibility(Sprite* this, uint8_t visibility) {
	this->_isVisible = visibility;
}

void Sprite_SetNoCollide(Sprite *this, uint8_t noCollide) {
	this->_noCollide = noCollide;
}

int Sprite_GetNoCollide(Sprite *this) {
	return this->_noCollide;
}

void Sprite_SetClickable(Sprite *this, uint8_t clickable) {
	this->_isClickable = clickable;
}

void Sprite_SetIsStatic(Sprite *this, int isStatic) {
	this->_isStatic = isStatic;
}

int Sprite_GetIsStatic(Sprite *this) {
	return this->_isStatic;
}



void Sprite_AddUpdateFunc(Sprite *this,
		void (*UpdateFunc)(void *this, uint32_t passedTime, void *args)) {
	UpdateFuncNode *currNode;
	currNode = this->_UpdateFunctions;
	if(currNode != NULL) {
		while(currNode->next != NULL) {
			currNode = currNode->next;
		}
		currNode->next = malloc(sizeof(UpdateFuncNode));
		currNode->next->Update = NULL;
		currNode->next->next = NULL;
		currNode->Update = UpdateFunc;
	}
	else {
		this->_UpdateFunctions = malloc(sizeof(UpdateFuncNode));
		this->_UpdateFunctions->Update = UpdateFunc;
		this->_UpdateFunctions->next = malloc(sizeof(UpdateFuncNode));
		this->_UpdateFunctions->next->Update = NULL;	
		this->_UpdateFunctions->next->next = NULL;
	}


}





int Sprite_AddTag(Sprite *this, const char *tagName, const void *tagData,
		int sizeOfData) {
	int returnVal = 0;	
	int tagExists;
	
	Tag *newTag;

	tagExists = Map_Count(&this->_tags, tagName);

	if(tagExists) {
		// The tag already exists, cannot be added.
		returnVal = -1;
	}
	else {
		// The tag doesn't exist.
		newTag = malloc(sizeof(Tag));
		newTag->data = malloc(sizeOfData);
		newTag->name = malloc(strlen(tagName) + 1);
		newTag->sizeOfData = sizeOfData;
		strcpy(newTag->name, tagName);
		memcpy(newTag->data, tagData, newTag->sizeOfData);
		
		Map_Set(&this->_tags, tagName, newTag);
	}

	return returnVal;

} // End Sprite_AddTag




int Sprite_ModifyTag(Sprite *this, const char *tagName, const void *tagData) {
	int returnVal = 0;
	Tag *currTag;

	currTag = Map_At(&this->_tags, tagName);

	if(currTag == NULL) {
		// The tag doesn't exist, so cannot be modifed.
		returnVal = -1;
	}
	else {
		// The tag does exist and can be modified
		memcpy(currTag->data, tagData, currTag->sizeOfData);
	}

	return returnVal;

} // End Sprite_ModifyTag




int Sprite_RemoveTag(Sprite *this, const char *tagName) {
	int returnVal = 0;

	if(Map_Count(&this->_tags, tagName)) {
		// The tag exists, so it can be removed.
		Map_Erase(&this->_tags, tagName);
	}
	else {
		// The tag didn't exist so it therefore can't be erased.
		returnVal = -1;	
	}

	return returnVal;

} // End Sprite_Removetag




int Sprite_DoesTagExist(Sprite *this, const char *tagName) {
	return (Map_Count(&this->_tags, tagName));
} // End Sprite_DoesTagExist




int Sprite_GetTagData(Sprite *this, const char *tagName, void *dataBuf,
		int sizeOfDataBuf, int *sizeOfData) {
	int returnVal = 0;
	Tag *currTag = Map_At(&this->_tags, tagName);

	if(currTag == NULL) {
		// Tha tag doesn't exist
		returnVal = -2;
	}
	else if(currTag->sizeOfData > sizeOfDataBuf) {
		// The tag exists, but the dataBuf is not big enough to hold all the
		// data.
		returnVal = -1;

		if(sizeOfData != NULL) {
			*sizeOfData = currTag->sizeOfData;
		}
	}


	else {
		// The tag exists, and teh dataBuf is large enough to hold the data
		memcpy(dataBuf, currTag->data, currTag->sizeOfData);	
		
		if(sizeOfData != NULL) {
			*sizeOfData = currTag->sizeOfData;
		}
	}

	
	

	return returnVal;

} // End Sprite_GetTagData









// Private method implementations

void _spriteDataParser(Sprite* this) {
	char buffer[1024];
	int i;
	char currChar = '\0';
	memset(buffer, '\0', sizeof(buffer));
	
	strcpy(buffer, this->_path);


	_parseSpriteNameFromPath(buffer, &buffer[0] + (sizeof(char) * 768));
	// Using a part of the buffer to fill in the spriteName from the path
	// that exists at the beginning of the buffer.
	// Will only fail if the paths exceed length limits set by the
	// O.S., so it cannot fail.

	this->_name = malloc(strlen( &buffer[0] + (sizeof(char) * 768) ) + 1);
	// + 1 for null term
	
	strcpy(this->_name, &buffer[0] + (sizeof(char) * 768));
	

	// Using the stem, get and parse the metadata file
	strcpy(buffer + strlen(buffer) - 4, SMPENGINE_SPRITE_DATA_EXT);
	// strlen(buffer) - 4 is the index of the '.' for the file extension
	// So now buffer contains the stem + SMPENGINE_SPRITE_DATA_EXT
	// buffer now contains "filePath" + ".ssd" which should be the path
	// to the meta data of the Sprite
	// Since an SDL_Rect is just 4 integers, an array of SDL_Rects is
	// simply an array of integers, so it can be treated as so to make
	// parsing and copying data simpler. 
	FILE* fp = fopen(buffer, "r");
	if(fp == NULL) {
		printf("File \"%s\" does not exist...%c", buffer, '\n');
		return;
	}
	// Get the number of Sprite parts from the file..
	i = 0;
	currChar = fgetc(fp);
	for(i = 0; currChar != '\n'; i++) {
		buffer[i] = currChar;
		currChar = fgetc(fp);
	}
	buffer[i] = '\0';
	this->_numParts = atoi(buffer);

	this->_partNames = malloc(this->_numParts * sizeof(char*));
	this->_partRectArray = malloc(this->_numParts * sizeof(SDL_Rect));
	// Allocates for the Sprite part names and their corresponding 
	// SDL_Rect

	int numIntsInSdlRect = 4;
	int buffIndex = 0; // Used for the index of buffer
	int rectIndex = 0; // The index into the SDL_Rects
	int numRects = 0; // The number of SDL_Rects in the file.
	int* SdlRectArray = (int*) this->_partRectArray;
	// Casting the SDL_Rect array as an integer array for easier and more
	// optimized logic while parsing.
	
	while(currChar != EOF) {
		currChar = fgetc(fp);

		if(currChar == ',') {
			buffer[buffIndex] = '\0';
			SdlRectArray[ (numRects * numIntsInSdlRect) + 
				rectIndex ] = atoi(buffer);
			rectIndex++;
			buffIndex = 0; // Reset the buffer
		}
		else if(currChar == ' ') {
			// Ignored..
		}
		else if(currChar == '\n') {
			// End of line, 'buffer' contains the part name
			// 'buffIndex' is the length of the part name
			// also points to where the null terminator should go
			
			if(buffIndex == 0) {
				// There was no name for the part...
				//
				// Printing warning and setting name to an 
				// empty string.
				printf("WARNING: No part name for Sprite %s "
						"part %d %c", this->_name,
						numRects, '\n');
				this->_partNames[numRects] = malloc(1);
				this->_partNames[numRects][0] = '\0';
			}
			else {
				// There was a part name
				buffer[buffIndex] = '\0';
				this->_partNames[numRects] = 
					malloc(buffIndex + 1);
				// 'buffIndex' is the size of the part name
				// + 1 for the null terminator
				strcpy(this->_partNames[numRects],
						buffer);
				// Copy the part name

			}


			numRects++; 
			// Line is parsed, increment the rect counter
			
			buffIndex = 0; // Reset the buffer
			rectIndex = 0; // Reset the index into SDL_Rect	
		}
		else if(numRects == this->_numParts) {
			// All the rects are account for, can break out of loop
			// This is here to prevent a trailing '\n' (newline)
			// from breaking the program.
			currChar = EOF; 
		}

		else {
			// The character is a digit or a character in the 
			// part name.
			buffer[buffIndex] = currChar;
			buffIndex++; 
		}
	}
	fclose(fp);

	this->_numParts = numRects;

			
}// End SpriteDataParser



void _parseSpriteNameFromPath(char* path, char* buffer) {
	int indexOfBuffer = 0;
	int indexOfPath = strlen(path) - 1;
	int lengthOfName = 0;
	// -1 to point to the last character
	
	while((path[indexOfPath] != '/') && (path[indexOfPath] != '\\')) {
		lengthOfName++;
		indexOfPath--; // Go back one character	
	}

	// Now the length of the name is in lengthOfName
	
	indexOfPath = strlen(path) - lengthOfName;
	// So it now points to the index of the beggining of the name.
	
	while(indexOfBuffer <= lengthOfName + 1) { // + 1 for null term
		buffer[indexOfBuffer] = path[indexOfPath];
		indexOfBuffer++;
		indexOfPath++;
	}
} // End _parseSpriteNameFromPath


void _defaultOnClick(Sprite* this, int mouseX, int mouseY, 
			void* additionalArgs) {};


void * Sprite_GetTagDataPtr(Sprite *this, const char *tagName) {
	void *returnVal = NULL;	
	
	Tag *tag = Map_At(&this->_tags, tagName);
	
	if(tag != NULL) {
		returnVal = tag->data;
	}

	return returnVal;

} // End Sprite_GetTagDataPtr



void Sprite_ClearSections(Sprite *this) {
	WorldSection *currSection, **spriteSections;
	int i;
	int numSections = this->_numSections;
	spriteSections = this->_sections;

	for(i = 0; i < numSections; i++) {
		currSection = spriteSections[i];	
		
		WorldSection_RemoveSprite(currSection, this);	
	}

	this->_numSections = 0;

} // End Sprite_ClearSections




void Sprite_AddSection(Sprite *this, WorldSection *section) {
	if(this->_numSections == this->_sizeOfSectionArr) {
		// The section array needs to be expanded.
		this->_sizeOfSectionArr *= 2;
		this->_sections = realloc(this->_sections, this->_sizeOfSectionArr);
	}

	this->_sections[this->_numSections] = section;
	this->_numSections++;
} // End Sprite_AddSection




void Sprite_CollisionHandler(Sprite *this, Sprite *other) {
	#ifdef DEBUG_COLLISIONS	
	printf("Sprite %d (%p) collided with Sprite %d (%p) \r\n", this->_regIndex,
			this, other->_regIndex, other);
	#endif
	
	if(this->OnCollision != NULL) {
		this->OnCollision(this, other);
	}
	if(other->OnCollision != NULL) {
		other->OnCollision(other, this);
	}

	//BasicPhys_CollisionHandler(&this->_props, &other->_props);

} // End Sprite_CollisionHandler



/*
void Sprite_SetWorldProps(Sprite *this, BasicWorldPhysProps *world) {
	this->_props.worldProps = world;
} // End Sprite_SetWorldProps
*/

// Macros specific to this function

#define Sprite_DistanceIntersectX(leftRect, rightRect) \
	((leftRect.x + leftRect.w) - rightRect.x)

#define Sprite_DistanceIntersectY(topRect, bottomRect) \
	((topRect.y + topRect.h) - bottomRect.y)

void Sprite_GetSidesOfCollision(Sprite *sprite1, Sprite *sprite2, 
		int *sideSprite1, int *sideSprite2) {
	SDL_Rect rect1, rect2;
	rect1 = sprite1->_boundingBox;
	rect2 = sprite2->_boundingBox;

// TODO: More testing on this
// generally works but corner cases need to be addressed

	if(Sprite_IsLeft(sprite1, sprite2)) {
		// sprite1 is to the left of sprite2

		if(Sprite_IsAbove(sprite1, sprite2)) {
			// sprite1 is to the left and above sprite2
			if(Sprite_DistanceIntersectX(rect1, rect2) > 
					Sprite_DistanceIntersectY(rect1, rect2)) {
				// sprite1 must have touched the top of sprite2
				*sideSprite1 = 2;
				*sideSprite2 = 0;
			}
			else {
				// sprite1 must have hit the left side of sprite2
				*sideSprite1 = 3;
				*sideSprite2 = 1;
			}
		}
		else {
			// sprite1 is to the left and below sprite2
			if(Sprite_DistanceIntersectX(rect1, rect2) > 
					Sprite_DistanceIntersectY(rect1, rect2)) {
				// sprite1 must have touched the bottom of sprite2
				*sideSprite1 = 0;
				*sideSprite2 = 2;
			}
			else {
				// sprite1 must have hit the left side of sprite2
				*sideSprite1 = 3;
				*sideSprite2 = 1;
			}
		}
	}
	else /*if(Sprite_IsRight(sprite1, sprite2))*/ {
		// sprite1 is to the right of sprite2

		if(Sprite_IsAbove(sprite1, sprite2)) {
			// sprite1 is to the right and above sprite2
			if(Sprite_DistanceIntersectX(rect1, rect2) > 
					Sprite_DistanceIntersectY(rect1, rect2)) {
				// sprite1 must have touched the top of sprite2
				*sideSprite1 = 2;
				*sideSprite2 = 0;
			}
			else {
				// sprite1 must have hit the right side of sprite2
				*sideSprite1 = 1;
				*sideSprite2 = 3;
			}
		}
		else {
			// sprite1 is to the right and below sprite2
			if(Sprite_DistanceIntersectX(rect1, rect2) > 
					Sprite_DistanceIntersectY(rect1, rect2)) {
				// sprite1 must have touched the bottom of sprite2
				*sideSprite1 = 0;
				*sideSprite2 = 2;
			}
			else {
				// sprite1 must have hit the right side of sprite2
				*sideSprite1 = 1;
				*sideSprite2 = 3;
			}
		}
	}



} // End Sprite_GetSidesOfCollision




int Sprite_ExistsAtCoords(Sprite *this, int x, int y) {

	return ((this->_boundingBox.x <= x) && ((this->_boundingBox.x + 
					this->_boundingBox.w) >= x)) && 
					((this->_boundingBox.y <= y) && ((this->_boundingBox.y + 
					this->_boundingBox.h) >= y));
} // End Sprite_ExistsAtCoords



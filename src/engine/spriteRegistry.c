#include "engine/spriteRegistry.h"
#include "game/sprites/sprite_hdr_db.h"
//#include <unistd.h>
#include <string.h>

#define SMPENGINE_DEFAULT_INITIAL_SIZE_REGISTRY 512
#define SMPENGINE_DEFAULT_INITIAL_SIZE_DATABASE 1024

// Forward declarations
int SpriteKaliDB_DestroySprite(Sprite *sprite);






// Function definitions

void SpriteRegistry_Init(SpriteRegistry *this, int initialSize, int regID) {
	if(initialSize < 0) {
		// Use default instead
		initialSize = SMPENGINE_DEFAULT_INITIAL_SIZE_REGISTRY;
	}
	this->_registry = malloc(sizeof(Sprite *) * initialSize);
	memset(this->_registry, 0, sizeof(Sprite *) * initialSize);
	this->_numRegSprites = 0;

	// Initialize emptyIndexStack
	this->_emptyIndexStack = malloc(sizeof(int) * initialSize);
	int i;
	for(i = 0; i < initialSize; i++) {
		this->_emptyIndexStack[i] = i;		
	}


	this->_registryID = regID;
	this->_registrySize = initialSize;

} // End SpriteRegistry_Init



void SpriteRegistry_Destroy(SpriteRegistry *this) {
	int i;
	int emptyRegIndices = 0;
	for(i = 0; i < this->_numRegSprites + emptyRegIndices; i++) {
		if(this->_registry[i] == NULL) {	
			emptyRegIndices++;
		}
		else {	
			SpriteKaliDB_DestroySprite(this->_registry[i]);
		}	
	}

	free(this->_registry);
	free(this->_emptyIndexStack);

} // End SpriteRegistry_Destroy



void SpriteRegistry_Reg(SpriteRegistry *this, Sprite *sprite) {
	int index;

	// Get the first empty index
	index = this->_emptyIndexStack[this->_numRegSprites];
	

#if SMPENGINE_DEBUG >= 1
	printf("Sprite %p has been registered at index %d\r\n", sprite, index);
#endif

	this->_registry[index] = sprite;
	this->_numRegSprites++;
	sprite->_regIndex = index; // The sprites index in the registyr is its ID

	if(this->_registrySize == this->_numRegSprites) {
		// The registry is now full. Needs to be expanded.
		int i;	
		int oldRegSize = this->_registrySize;		
		this->_registrySize = this->_registrySize * 2;
	
		this->_registry = realloc(this->_registry, sizeof(Sprite *) * 
				this->_registrySize); // Doubles the registry size
		this->_emptyIndexStack = realloc(this->_emptyIndexStack, sizeof(int) *
				this->_registrySize); // Doubles the emptyindexstack

		// Initialize the rest of the index stack
		for(i = oldRegSize; i < this->_registrySize; i++) {
			this->_emptyIndexStack[i] = i;
		}
	}

} // End SpriteRegistry_RegSprite



void SpriteRegistry_Unreg(SpriteRegistry *this, int regID) {
#if SMPENGINE_DEBUG >= 1
	printf("Sprite %p has been unregistered at index %d\r\n",
		 this->_registry[regID], regID);
#endif
	this->_registry[regID]->_regIndex = -1;	
	this->_registry[regID] = NULL;
	// Moves back the registry "ptr"
	this->_numRegSprites--;

	// Add the recently removed registry index into the empty registry index 
	// stack

	this->_emptyIndexStack[this->_numRegSprites] = regID;


} // End SpriteRegistry_Unreg




void SpriteRegistry_DrawAll(SpriteRegistry *this, Graphics *graphics,
		SDL_Rect *offset) {
	int i;
	int emptyIndices = 0;
	for(i = 0; i < this->_numRegSprites + emptyIndices; i++) {
		if(this->_registry[i] == NULL) {
			emptyIndices++;	
		}
		else {
			Sprite_Draw(this->_registry[i], graphics, offset);
		}	
	}
} // End SpriteRegistry_DrawAll
	



void SpriteRegistry_UpdateAll(SpriteRegistry *this, uint32_t passedTicks, 
		void *instance) {
	int i;
	int emptyIndices = 0;
	for(i = 0; i < this->_numRegSprites + emptyIndices; i++) {
		if(this->_registry[i] == NULL) {
			emptyIndices++;
		}
		else {
#if SMPENGINE_DEBUG >= 2
			printf("Updating sprite %p at index %d\r\n", this->_registry[i], i);
#endif	
			Sprite_Update(this->_registry[i], passedTicks, this, instance);
		}
	}
} // End Sprite_UpdateAll



const Sprite ** SpriteRegistry_GetPtrToIndex(SpriteRegistry *this, int regID) {
	return (Sprite const **) &this->_registry[regID];
}

















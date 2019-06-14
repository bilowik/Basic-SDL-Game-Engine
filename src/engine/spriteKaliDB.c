#include "engine/spriteKaliDB.h"
#include "game/sprites/sprite_hdr_db.h"


// Private structure to hold sprite kalis
typedef struct SpriteKali SpriteKali;
struct SpriteKali {
	void (*Init)(void *sprite, SpriteRegistry *registry,
			int id, Graphics *graphics);
	void (*Destroy)(void *sprite);
	int id;
	int sizeOfSprite;
};


// Global SpriteKaliDB
static struct {
	Mapi _spriteKalis;
} spriteKaliDB;






void SpriteKaliDB_Init(int initialSize) {
	Mapi_Init(&spriteKaliDB._spriteKalis, sizeof(SpriteKali), initialSize);

#include "game/sprites/spritedb.inc"
}



void SpriteKaliDB_Destroy() {
	Mapi_Destroy(&spriteKaliDB._spriteKalis);
}





int SpriteKaliDB_AddSpriteKaliToDB(int spriteID, int sizeOfSprite,
		void (*Constructor)(void *sprite,
			SpriteRegistry *registry, int id, Graphics *graphics), 	
		void (*Destructor)(void *sprite)) {
	SpriteKali spriteKali;	
	int idTaken;
	int returnVal = 0;
	idTaken = Mapi_Count(&spriteKaliDB._spriteKalis, spriteID);

	if(idTaken) {
		printf("ERROR: ID %d is already taken!\r\n", spriteID);
		returnVal = -1;
	}
	else {
		// ID wasn't taken, so it is added.
		spriteKali.id = spriteID;
		spriteKali.sizeOfSprite = sizeOfSprite;
		spriteKali.Init = Constructor;
		spriteKali.Destroy = Destructor;	
		Mapi_Set(&spriteKaliDB._spriteKalis, spriteID, &spriteKali);
	}


	return returnVal;
} // End SpriteKaliDB_AddSpriteInitFuncToDB




Sprite *SpriteKaliDB_BuildSpriteByID(SpriteRegistry *reg, int spriteID, 
		Graphics *graphics) {
	SpriteKali *spriteKali;
	Sprite *newSprite;
	spriteKali = Mapi_At(&spriteKaliDB._spriteKalis, spriteID);

	if(spriteKali == NULL) {
		// There doesn't exist sprite with that ID

	}
	else { 
		// Sprite exists with that id
		newSprite = malloc(spriteKali->sizeOfSprite);
		spriteKali->Init(newSprite, reg, spriteID, graphics);
	}

	return newSprite;

} // End SpriteKaliDB_BuildSpriteByID




int SpriteKaliDB_DestroySprite(Sprite *sprite) {
	int returnVal = 0;	
	SpriteKali *spriteKali = Mapi_At(&spriteKaliDB._spriteKalis,
			sprite->_spriteID);

	if(spriteKali == NULL) {
		// The sprite ID did not exist in the mapi
		returnVal = -1;
	}
	else {
		// The sprite ID was valid.
		spriteKali->Destroy(sprite);
	}

	return returnVal;

} // End SpriteKaliDB_DestroySprite

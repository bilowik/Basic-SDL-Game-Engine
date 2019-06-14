#include "physBall.h"
#include "engine/globals.h"
#include "game/test/instance.h"

static const char PATH[] = "/sprites/ballSheet.png";


void PhysBall_Init(void *thisPtr, SpriteRegistry *reg, int id, 
		Graphics *graphics) {
	char *path = malloc(strlen(&PATH[0]) + strlen(G_RESOURCE_PATH) + 1);
	strcpy(path, G_RESOURCE_PATH);
	strcat(path, &PATH[0]);


	PhysBall *this = thisPtr;
	Sprite_Init(thisPtr, reg, path, graphics);

	free(path);

	memset(&this->_base._props, 0, sizeof(BasicPhysProps));
	this->_base._props.weight = 5.0f;
	this->_base._props.friction = 0.1f;

	this->_base._isStatic = 0;			
	this->_base._spriteID = id;
			
	this->_base._boundingBox.x = 0;
	this->_base._boundingBox.y = 0;
	this->_base._boundingBox.w = 32;
	this->_base._boundingBox.h = 32;

	Sprite_AddUpdateFunc((Sprite *) this, PhysBall_UpdatePhys);
}



void PhysBall_Destroy(void *thisPtr) {
	Sprite_Destroy(thisPtr);
}



void PhysBall_UpdatePhys(void *this, uint32_t passedTicks, void *instance) {
	PhysBall *physBall = this;
	Instance *inst = instance;	
	BasicPhysProps *props = &physBall->_base._props;

	BasicPhys_UpdateObj(&inst->worldProps, props, passedTicks);
	physBall->_base._boundingBox.x = props->posX;
	physBall->_base._boundingBox.y = props->posY;

}




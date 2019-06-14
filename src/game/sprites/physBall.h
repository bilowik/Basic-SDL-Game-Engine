#ifndef SMP_ENGINE_BALL_H
#define SMP_ENGINE_BALL_H

#include "engine/basicPhys.h"
#include "engine/sprite.h"

typedef struct PhysBall PhysBall;

struct PhysBall {
	Sprite _base;
}; 


void PhysBall_Init(void *thisPtr, SpriteRegistry *reg, int id, 
		Graphics *graphics);

void PhysBall_Destroy(void *thisPtr);

void PhysBall_UpdatePhys(void *this, uint32_t passedTicks, void *instance);

#endif

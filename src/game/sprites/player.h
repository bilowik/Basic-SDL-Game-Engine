#ifndef SMPENGINE_PLAYER_H
#define SMPENGINE_PLAYER_H

#include "engine/sprite.h"
#include "engine/animatedSprite.h"

typedef struct Player Player;

struct Player {
	AnimatedSprite _base;

	float _groundMovementAcc;
	float _groundMovementMaxVel;

	float _airMovementAcc;

	uint8_t _justJumped : 1; // Jumped in the last frame.
	uint8_t _movingLeft : 1;
	uint8_t _movingRight : 1; 
	uint8_t _grounded : 1; // Is on a solid surface
	

}; 




void Player_Init(void *thisPtr, SpriteRegistry *reg, int id, 
		Graphics *graphics);

void Player_Destroy(void *thisPtr);

void Player_Update(void *this, uint32_t passedTicks, void *instance);

void Player_Jump(Player *this);

void Player_MoveRight(Player *this, uint32_t passedTicks);

void Player_MoveLeft(Player *this, uint32_t passedTicks);

void Player_StopMoveRight(Player *this);

void Player_StopMoveLeft(Player *this);



void Player_OnCollision(void *thisSprite, void *otherSprite);










#endif

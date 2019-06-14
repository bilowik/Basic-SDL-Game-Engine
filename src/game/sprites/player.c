#include "game/sprites/player.h"
#include "engine/globals.h"
#include "game/test/instance.h"


static char const PATH[] = "/sprites/player_placeholder.png";
//static char const PATH[] = "/sprites/sonic.png";

void Player_Init(void *thisPtr, SpriteRegistry *reg, int id,
		Graphics *graphics) {
	Player *this = thisPtr;
	char *path = malloc(strlen(&PATH[0]) + strlen(G_RESOURCE_PATH) + 1);
	strcpy(path, G_RESOURCE_PATH);
	strcat(path, &PATH[0]);

	AnimatedSprite_Init(thisPtr, reg, path, graphics);

	free(path);

	memset(&((Sprite *) thisPtr)->_props, 0, sizeof(BasicPhysProps));
	((Sprite *) thisPtr)->_props.weight = 10.0f;
	((Sprite *) thisPtr)->_props.friction = 0.01f;
	((Sprite *) thisPtr)->_props.grounded = 0;
	this->_groundMovementMaxVel = 0.5f;
	this->_groundMovementAcc = 0.005f;
	this->_airMovementAcc = 0.0005f;

	((Sprite *) thisPtr)->_isStatic = 0;
	((Sprite *) thisPtr)->_spriteID = id;

	((Sprite *) thisPtr)->_boundingBox.x = 0;
	((Sprite *) thisPtr)->_boundingBox.y = 0;
	((Sprite *) thisPtr)->_boundingBox.w = 32;
	((Sprite *) thisPtr)->_boundingBox.h = 32;

	this->_justJumped = 0;
	this->_movingLeft = 0;
	this->_movingRight = 0;
	((Sprite *) this)->_isVisible = 1;
	((Sprite *) thisPtr)->_props.grounded = 0;
	Sprite_AddUpdateFunc((Sprite *) this, Player_Update);

	((Sprite *) thisPtr)->OnCollision = Player_OnCollision;
} // End Player_Init

void Player_Destroy(void *thisPtr) {
	Sprite_Destroy(thisPtr);
}


void Player_Update(void *thisPtr, uint32_t passedTime, void *args) {
	Player *this = thisPtr;
	Sprite *sprite = thisPtr;
	Instance *inst = args;

	if(this->_movingRight) {
		if(sprite->_props.grounded) {
			sprite->_props.accX = this->_groundMovementAcc;
		}
		else {
			sprite->_props.accX = this->_airMovementAcc;
		}
	}
	else if(this->_movingLeft) {
		if(sprite->_props.grounded) {
			sprite->_props.accX = 0 - this->_groundMovementAcc;
		}
		else {
			sprite->_props.accX = 0 - this->_airMovementAcc;	
		}
	}
	else {
		// The player is neither accelerating left or right.
		sprite->_props.accX = 0;
		AnimatedSprite_StopAnimation(thisPtr);
	}
	if(sprite->_props.grounded) {
		sprite->_props.friction = 15.0f;
	}



	Sprite_UpdatePhys((Sprite *) this, &inst->worldProps, passedTime);

	if(this->_justJumped) {
		this->_justJumped = 0;
		sprite->_props.accY = 0;
		
	}


	if(ABS_VAL(sprite->_props.velX) > this->_groundMovementMaxVel) {
		if(sprite->_props.velX > 0) {
			// It's moving right and has hit max velocity
			sprite->_props.velX = this->_groundMovementMaxVel;
		}
		else {
			// It's moving left and has hit max velocity
			sprite->_props.velX = 0 - this->_groundMovementMaxVel;
		}
	}

	sprite->_props.grounded = 0;		
	sprite->_props.friction = 1.0f;

}




void Player_Jump(Player *this) {
	if(((Sprite *) this)->_props.grounded) {
		this->_justJumped = 1;
		BasicPhys_Accelerate(&((Sprite *) this)->_props, 270, DEGREES_T, 0.5f);
	}
}

void Player_MoveRight(Player *this, uint32_t passedTicks) {
	this->_movingRight = 1;
	AnimatedSprite_SetAnimation((AnimatedSprite *) this, 0);
	AnimatedSprite_StartAnimation((AnimatedSprite *) this, passedTicks);
	Sprite_SetFlip((Sprite *) this, SDL_FLIP_NONE);
}

void Player_MoveLeft(Player *this, uint32_t passedTicks) {
	this->_movingLeft = 1;
	AnimatedSprite_SetAnimation((AnimatedSprite *) this, 0);
	AnimatedSprite_StartAnimation((AnimatedSprite *) this, passedTicks);
	Sprite_SetFlip((Sprite *) this, SDL_FLIP_HORIZONTAL);
}

void Player_StopMoveRight(Player *this) {
	this->_movingRight = 0;
	if(this->_movingLeft == 0) {
		AnimatedSprite_StopAnimation((AnimatedSprite *) this);
	}
}

void Player_StopMoveLeft(Player *this) {
	this->_movingLeft = 0;
	if(this->_movingRight == 0) {
	AnimatedSprite_StopAnimation((AnimatedSprite *) this);
	}
}
		

void Player_OnCollision(void *thisSprite, void *otherSprite) {
	int sideSprite1, sideSprite2;
	//Player *this = thisSprite;
	Sprite *other = otherSprite;
	Sprite_GetSidesOfCollision(thisSprite, otherSprite, &sideSprite1,
			&sideSprite2);

	if(sideSprite1 == 2) {
		// The player has landed on a surface
		((Sprite *) thisSprite)->_props.grounded = 1;
		((Sprite *) thisSprite)->_props.accY = 0;
		((Sprite *) thisSprite)->_props.velY = 0;
		Sprite_SetPrecisePos(thisSprite, ((Sprite *) thisSprite)->_props.posX,
				other->_boundingBox.y - 
				((Sprite *) thisSprite)->_boundingBox.h);
	}

} // End Player_OnCollision

	
	


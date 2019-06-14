#include "engine/basicPhys.h"
#include <math.h>

static double const MY_PI = 3.141529265358979323846264338327f;




void BasicPhysProps_Init(BasicPhysProps *this) {
	this->weight = 1.0f;
	this->posX = 0.0f;
	this->posY = 0.0f;
	this->velX = 0.0f;
	this->velY = 0.0f;
	this->accX = 0.0f;
	this->accY = 0.0f;
	this->rotationalVel = 0.0f;
	this->friction = 1.0f;
	this->bounciness = 1.0f;
}




void BasicPhys_ApplyGravity(BasicWorldPhysProps *phys, BasicPhysProps *obj,
		uint32_t passedTicks) {
	obj->velY = obj->velY + (phys->gravity * passedTicks);
}





void BasicPhys_UpdateObj(BasicWorldPhysProps *phys, BasicPhysProps *obj,
		uint32_t passedTicks) {
	float gravity;
	if(obj->grounded) {
		gravity = 0;
	}
	else {
		gravity = phys->gravity;
	}

	
	// Apply current acceleration:
	obj->velX = obj->velX + (obj->accX * passedTicks);
	obj->velY = obj->velY + (obj->accY * passedTicks);
	
	// Apply gravity:
	obj->velY = obj->velY + (gravity * passedTicks);

	// Apply generic friction
	obj->velX = obj->velX - (phys->friction * obj->friction * passedTicks * 
			obj->velX);
	obj->velY = obj->velY - (phys->friction * obj->friction * passedTicks * 
			obj->velY);
	
	
	// Update position based on new velocities.
	obj->posX = obj->posX + (obj->velX * passedTicks);
	obj->posY = obj->posY + (obj->velY * passedTicks);
} // End BasicPhys_UpdateObj



void BasicPhys_Accelerate(BasicPhysProps *this, float angle, 
		Angle_Type angleType, float acceleration) {
	float angleInRadians;
	if(angleType == RADIANS_T) {
		angleInRadians = angle;
	}
	else {
		// The angle is in degrees, conversion required.
		angleInRadians = (angle * MY_PI) / 180;
	}

	this->velX += cosf(angleInRadians) * acceleration;
	this->velY += sinf(angleInRadians) * acceleration;
		


} // End BasicPhys_Accelerate






void BasicPhys_CollisionHandler(BasicPhysProps *props1, 
		BasicPhysProps *props2) {

}






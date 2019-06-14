#ifndef SMP_ENGINE_BASICPHYS_H
#define SMP_ENGINE_BASICPHYS_H
/* Basic physics impelementation
	An object should have a BasicPhysProps attached to it if it is a physical 
	object.

	Upon each frame, BasicPhys_UpdateObjs(int passedTicks); is called, and 
	for each physics object that is currently loaded, the changes in their 
	physics (velocity, rotational velocity, etc.)

*/


#include <stdint.h>

// Enums
enum Angle_Type {
	RADIANS_T,
	DEGREES_T
};

extern char const *G_BASIC_PHYS_TAG_NAME;


typedef struct BasicPhysProps BasicPhysProps;
typedef struct BasicWorldPhysProps BasicWorldPhysProps;
typedef enum Angle_Type Angle_Type;



struct BasicPhysProps {
	// To be attached to objects.
	
	float weight;
	// Weight of an object, determines how affected it is by gravity.
	
	float posX;
	float posY;
	// The position of the obj.

	float velX;
	float velY;
	// Current velocities, if any.

	float accX;
	float accY;
	// Current acceleration, if any.

	float rotationalVel;
	// The rotational velocity of the obj

	float friction;
	// The amount of friction this object imparts.
	// Counts towards itself and other objects interacting with it.

	float bounciness; //TODO: Impelment
	// "Rubber bouncy ball effect"ness

	
	uint8_t grounded : 1; // Touching a solid object, meaning no pulling 
	// from gravity.	
	
	//BasicWorldPhysProps *worldProps;
	// A pointer to the given world properties

	
};


struct BasicWorldPhysProps {
	// The physics associated with the world.
	// Meant to be a single global instance
	
	float gravity;
	// The rate at which objects are pulled downwards.
	
	float gravityDirection;
	// The direction at which gravity pulls.
	// [0,2pi], being straight down, is the default.
	
	float friction;
};


void BasicPhysProps_Init(BasicPhysProps *this);



void BasicPhys_ApplyGravity(BasicWorldPhysProps *phys, BasicPhysProps *obj,
		uint32_t passedTicks);
// Purpose	:	Calculates the new given velocity of a physics object given a
// 				world physics set of properties using it's gravity and the objs
// 				weight.
// Pre		:	phys and obj are not null
// Post		:	The new velocity is set on the obj.

void BasicPhys_UpdateObj(BasicWorldPhysProps *phys, BasicPhysProps *obj,
		uint32_t passedTicks);
// Purpose	:	Performs all necessary physics calculations for the given obj
// 				and the given world physics, and changes the physical properties
// 				accordingly.
// Pre		:	phys and obj are not null. 
// Post		:	phys properties are changed accordingly.

void BasicPhys_Accelerate(BasicPhysProps *this, float angle,
		Angle_Type angleType, float acceleration);
// Purpose	:	Adjusts the velocity in the x and y direction based on 
// 				the given direction and acceleration. 
// 				(Adds one tick of acceleration in the given direction)
// Pre		:	N/A
// Post		:	The velocity has been adjusted




void BasicPhys_CollisionHandler(BasicPhysProps *props1,
		BasicPhysProps *props2);
// Purpose	:	Does physics calcuations on the collision between these two 
// 				objects.
// Pre		:	props are Initialization
// Post		:	props are updated.




#endif

#ifndef SMPENG_ANIMATEDSPRITE_H
#define SMPENG_ANIMATEDSPRITE_H
#include <stdint.h>
#include "sprite.h"

// Constants
static const char SMPENGINE_ANIMATION_DATA_EXT[] = ".anim";

typedef struct animationFrameData animationFrameData;

struct animationFrameData {
	int indexOfSpritePart; // the sprite part for this frame.	
	uint16_t lengthOfFrame; // in milisecs
}; // End animationFrameData	



typedef struct animation animation;

struct animation {
	animationFrameData* frames;
	float animationSpeedMultipler;
	// The speed multiplier for this specific animation
	// DEFALT = 1.0f
	
	uint16_t numFrames;
	uint8_t once;
	// 0 if the animation loops until 'pause' or 'stop'
	// 1 if the animation plays once 
	
	char* name;
};


typedef struct AnimatedSprite AnimatedSprite;

struct AnimatedSprite /* Inherits from Sprite */ {
	// Inherited class and methods SEE sprite.h for documentation
	Sprite _base; // The inherited class

	// New Fields
	
	animation* _currAnimation;
	// Pointer to the currently used animation for the sheet
	// which is a array of animationFrameData structs.	
	
	animation* _animations;
	// Pointer to an array of animationFrameData arrays, which
	// each array being an individual animation.

	int _numAnimations;
	// The total number of animations for this sprite
	
	int _currFrame;
	// The current frame of the current animation.


	//uint32_t  _timeWhenLastAnimUpdate;
	// Contains the time when the current animation 
	// advanced to the current frame.
	// Set by '_startAnimation' 
	// Updated by '_update'
	

	uint32_t _timeIntoCurrentAnim;
	// The time that was left over after '_update'
	// If enough time didn't go by to davance a frame, that remaining 
	// time is saved in this field.
	
        uint32_t _timeWhenPaused;
	// = 0 if not paused
	// = the global time when the animation was paused
	

	float _globalAnimationSpeedMultiplier;
	// DEFAULT = 1.0f
	// Used to increase/decrase the overall speed of an animation


	uint8_t _isPlaying : 1; // 1 if animation is playing, 0 if not.
	uint8_t _isPaused : 1; // 1 if the animation is paused, 0 if not.
		


	// New methods

	





}; // End AnimatedSprite class

// Constructors
void AnimatedSprite_Init(AnimatedSprite* this, SpriteRegistry *registry, 
		const char* filePath, Graphics* graphics);


// Destructor
void AnimatedSprite_Destroy(void *thisPtr);





void AnimatedSprite_Update(void* this,  uint32_t passedTicks, void *args);
// Purpose	:	Updates the currently played animation
// Pre		:	'AnimatedSprite' has been initialized.
// 			'_currAnimation' is not NULL and points to
// 			a valid animation.
// Post		:	'_currFrame' is incremented if enough time
// 			has passed.

void AnimatedSprite_StartAnimation(AnimatedSprite* this, uint32_t passedTicks);
// Purpose	:	Starts the current animation or resumes a 
// 			paused one.
// 			Can also ''restart'' the animation currently
// 			playing.
// Pre		:	'AnimatedSprite' is initialized.
// 			'_currAnimation' points to a valid animation.
// Post		:	'update()' will now process the current 
// 			animation and update the currentsprite part.
				
void AnimatedSprite_PauseAnimation(AnimatedSprite* this, uint32_t passedTicks);
// Purpose	:	Pauses the current animation
// Pre		:	'AnimatedSprite' has been initialized.
// 			An animation was playing.
// Post		:	'update' will not update the current sprite part.
// 			Will still be rendered to the screen unless 
// 			_isVisible is set.

void AnimatedSprite_StopAnimation(AnimatedSprite* this);
// Purpose	:	Stops the current animation
// Pre		:	'AnimatedSprite' is initialized.j
// Post		:	'_isPlaying' is set to 0.
// 			'update' no longer changes the sprite part
// 			unless 'startAnimation' is called again.

void AnimatedSprite_SetAnimation(AnimatedSprite* this, int indexOfAnimation);
// Purpose	:	Sets the current animation.
// Pre		:	'AnimatedSprite' is initialized. 
// 			An animatin is NOT currently playing.
// Post		:	'startAnimation()' can now be called to start
// 			the current animation.	


#endif

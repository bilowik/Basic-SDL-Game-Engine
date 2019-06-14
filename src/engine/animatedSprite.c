#include "animatedSprite.h"
#include <stdio.h>


// Private methods
static void _animationDataParser(AnimatedSprite* this);




// Constructors

void AnimatedSprite_Init(AnimatedSprite* this, SpriteRegistry *registry, 
		const char* filePath, Graphics* graphics) {
	// Initialize the parent class
	Sprite_Init(&this->_base, registry, filePath, graphics);
	Sprite_AddUpdateFunc(&this->_base, AnimatedSprite_Update);
	_animationDataParser(this);

	this->_currFrame = 0;
	//this->_timeWhenLastAnimUpdate = 0;
	this->_timeIntoCurrentAnim = 0;
	this->_timeWhenPaused = 0;
	this->_globalAnimationSpeedMultiplier = 1.0f;
	this->_isPlaying = 0;
	this->_isPaused = 0;
	
} // End AnimatedSprite_Init




void AnimatedSprite_Destroy(void *thisPtr) {
	AnimatedSprite *this = thisPtr;
	Sprite_Destroy(&this->_base);

	int i;
	for(i = 0; i < this->_numAnimations; i++) {
		free(this->_animations[i].frames);
		free(this->_animations[i].name);
	}
	free(this->_animations);
	this->_currAnimation = NULL;
	this->_animations = NULL;


} // End AnimatedSprite_Destroy




void AnimatedSprite_Update(void *this, uint32_t passedTicks, void *args) {
	AnimatedSprite *animSprite = this;
	if(animSprite->_isPlaying && !animSprite->_isPaused) {
		uint32_t timePassedDuringAnim = passedTicks + 
			animSprite->_timeIntoCurrentAnim;


		if(timePassedDuringAnim >= animSprite->_currAnimation->frames
				[animSprite->_currFrame].lengthOfFrame / 
				animSprite->_globalAnimationSpeedMultiplier) {
		
			while(timePassedDuringAnim >= animSprite->_currAnimation->frames
					[animSprite->_currFrame].lengthOfFrame / 
					animSprite->_globalAnimationSpeedMultiplier) {
				// Enough time has passed to advance to the next frame
				// of the animation.
				// Loop will continue to advance frames if enough time
				// has passed.
				
				if( (animSprite->_currFrame == animSprite->_currAnimation->
							numFrames - 1) && 
						animSprite->_currAnimation->once ) {
					// The animation is on it's final frame and
					// it does not loop, so it's completed.
					animSprite->_currFrame = 0;
					animSprite->_isPlaying = 0;
					animSprite->_timeIntoCurrentAnim = 0;
					//animSprite->_timeWhenLastAnimUpdate = 0;
					break;

				}
				else {
					// The animation wasn't on it's final frame.
					timePassedDuringAnim -= 
						animSprite->_currAnimation->frames
						[animSprite->_currFrame].lengthOfFrame /
						animSprite->_globalAnimationSpeedMultiplier;
					
					animSprite->_currFrame++;

					if(animSprite->_currFrame == 
							animSprite->_currAnimation->numFrames) {
						// The animation advanced past it's
						// final frame.
						animSprite->_currFrame = 0;
					}

					animSprite->_base._currPart = 
						animSprite->_currAnimation->frames
						[animSprite->_currFrame].indexOfSpritePart;
					// Gets the index of the sprite part from 
					// the animation structure.
				}

			} // End while( timePassed... )


		}
	
		animSprite->_timeIntoCurrentAnim = timePassedDuringAnim;
		/*
		animSprite->_timeIntoCurrentAnim = timePassedDuringAnim;
		// This gets the leftover time remaining that wasn't enough
		// to advance to the next frame. Is added to the time passed
		// when updating again.
		
		animSprite->_timeWhenLastAnimUpdate = passedTicks;
		*/
	} // end if(isPlaying)


} // End AnimatedSprite_Update




void AnimatedSprite_StartAnimation(AnimatedSprite* this, uint32_t passedTicks) {
	this->_isPlaying = 1;

	if(this->_isPaused) {
		this->_isPaused = 0;
	}
	else {
		this->_currFrame = 0;
		this->_timeIntoCurrentAnim = 0;
		this->_timeWhenPaused = 0;

		this->_base._currPart = 
			this->_currAnimation->frames
			[this->_currFrame].indexOfSpritePart;
	}

	//this->_timeWhenLastAnimUpdate = passedTicks;
} // End AnimatedSprite_StartAnimation




void AnimatedSprite_PauseAnimation(AnimatedSprite* this, uint32_t passedTicks) {
	this->_timeWhenPaused = passedTicks;
	this->_isPaused = 1;
} // End AnimatedSprite_PauseAnimation



void AnimatedSprite_StopAnimation(AnimatedSprite* this) {
	this->_isPlaying = 0;
	this->_isPaused = 0;
	this->_base._currPart = 0;
	this->_timeIntoCurrentAnim = 0;

} // End AnimatedSprie_StopAnimation




void AnimatedSprite_SetAnimation(AnimatedSprite* this, int indexOfAnimation) {	
	this->_currAnimation = &this->_animations[indexOfAnimation];
} // End AnimatedSprite_SetAnimation





void _animationDataParser(AnimatedSprite* this) {
	char buffer[1024];
	char currChar;
	FILE* fp;
	int i;
	int animationCount = 0;
	int frameCount = 0;


	strcpy(buffer, this->_base._path);
		
	strcpy(&buffer[0] + strlen(buffer) - 4, SMPENGINE_ANIMATION_DATA_EXT);
	// strlen(buffer) - 4 is the index of the '.' for the file extension
	// So now buffer contains the stem + SMPENGINE_ANIMATION_DATA_EXT
	
	fp = fopen(buffer, "r");
	
	if(fp == NULL) {
		printf("FILE \"%s\" does not exist...%c", buffer, '\n');
		return;
	}
	
	// Get the number of animations in the file
	i = 0;
	currChar = fgetc(fp);
	for(i = 0; currChar != '\n'; i++) {
		buffer[i] = currChar;
		currChar = fgetc(fp);
	}
	buffer[i] = '\0'; // append a null term
	this->_numAnimations = atoi(buffer);

	this->_animations = malloc(sizeof(animation) * this->_numAnimations);

	// line states
	uint8_t animName = 1;

	int buffIndex = 0;
	while(animationCount < this->_numAnimations) {
		while(currChar == '\n') {
			// Clear empty lines between animation data
			currChar = fgetc(fp);
		}


		// *** GETING THE NAME
		while(animName) {
			
			if(currChar == ':') {
				// It's the end of the name
				buffer[buffIndex] = '\0';
				this->_animations[animationCount].name = 
					malloc(buffIndex + 1);
				// buffIndex contains the length of the name
			
				strcpy(this->_animations[animationCount].name,
						buffer);

				buffIndex = 0;
				animName = 0;

			}
			else {

				buffer[buffIndex] = currChar;
				buffIndex++;
			}

			currChar = fgetc(fp);

		}



		// *** GETTING THE BOOLEAN ONCE 
		//
		// Just finished reading in the name, the next character is 
		// a 0, or 1 (if there was only one space), 
		// boolean values for animation.once if the
		currChar = fgetc(fp);
		while(currChar == ' ') {
			// Ignore any whitespace
			currChar = fgetc(fp);
		}
		// currChar now equals a 0 or 1, to convert, subtract 48 (ascii
		// offset of the numeral characters)
		this->_animations[animationCount].once = currChar - 48;


		
		// *** GETTING THE NUMBER OF FRAMES
		//
		// Next number on this line will be the number of frames.
		currChar = fgetc(fp);
		while( (currChar == ',') || (currChar == ' ') ) {
		  	// Ignore whitespace and commas	
			currChar = fgetc(fp);
		}

		// currChar now holds the first character to the number
		// of frames for the current animation
		while(currChar != '\n') {	
			buffer[buffIndex] = currChar;
			buffIndex++;	
			currChar = fgetc(fp);

		}
		buffer[buffIndex] = '\0'; // Append null term
		// buffer now contains the number of frames

		this->_animations[animationCount].numFrames = atoi(buffer);
		this->_animations[animationCount].frames = 
			malloc(this->_animations[animationCount].numFrames *
					sizeof(animationFrameData));


		// *** GETTING THE FRAME DATA
		// Now the next lines contain the animation frame data	
		frameCount = 0;
		while(frameCount < this->_animations[animationCount].numFrames) {
			buffIndex = 0; // reset the buffer index
			currChar = fgetc(fp); // Get the first character
			while(currChar != ',') {
				// currChar is part of a number
				buffer[buffIndex] = currChar;
				buffIndex++;	
				currChar = fgetc(fp);
			}
			buffer[buffIndex] = '\0'; // Append null term

			// buffer now contains the index of the sprite part of
			// the frame

			this->_animations[animationCount].frames[frameCount].
				indexOfSpritePart = atoi(buffer);

			buffIndex = 0; // Reset buffer index

			currChar = fgetc(fp);
			while((currChar == ' ') || currChar == ',') {
				// Ignore whitespace
				currChar = fgetc(fp);
			}

			// currChar now has the first character in the nmber
			// of miliseconds for the frame.

			while(currChar != '\n') {
				buffer[buffIndex] = currChar;
				buffIndex++;
				currChar = fgetc(fp);
			}
			buffer[buffIndex] = '\0'; // Append null term
			
			this->_animations[animationCount].frames[frameCount].
				lengthOfFrame = atoi(buffer);

			// This frame of the animation is done.
			// currChar == '\n'
			frameCount++;

		}
		animName = 1; // The next non-empty line should be a name.
		animationCount++;
	}


	fclose(fp);
} // End _animationDataParser





			
				

		











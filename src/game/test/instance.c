#include "game/test/instance.h"
#include "engine/plainText.h"
#include "game/sprites/crosshair.h"
#include "game/sprites/physBall.h"
#include "game/sprites/player.h"
#include "engine/overlay.h"
#include "engine/overlayElement.h"
#include "engine/overlayImage.h"
#include "game/test/debugOverlay.h"
#include <time.h>

#define WIN_SIZE_WIDTH 1280
#define WIN_SIZE_HEIGHT 720
#define FPS 144
#define INTERNAL_FPS 240
#define TICKS_PER_FRAME  1000 / FPS
#define INTERNAL_TICKS_PER_FRAME 1000 / INTERNAL_FPS
#define INITIAL_TEXTUREDB_SIZE 128
#define SMPENGINE_DEFAULT_NUM_LAYERS 12
#define SMPENGINE_DEFAULT_LAYER_SIZE 500
#define SPRITE_SCALE_BASE_WIDTH 1280
#define SPRITE_SCALE_BASE_HEIGHT 720

// World constants
#define WORLD_SIZE_X 8192
#define WORLD_SIZE_Y 2048
#define SECTION_SIZE 128
#define SPRITES_PER_SECTION 10

int Instance_Init(Instance *this, int argc, char **argv, int numLayers, 
		int layerSize) {
	char * currWorkingDir;
	char contentPath[] = "/content";
	char defaultTexturePath[] = "/sprites/defaultTexture.png";
	char defaultFontPath[] = "/fonts/font.ttf";
	int i;
	
	// Initialize SDL	
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();


	// Initialize paths
	currWorkingDir = FilePathHandler_GetCurrWorkingDir(argv[0]);	
	G_RESOURCE_PATH = malloc(strlen(currWorkingDir) + strlen(&contentPath[0]) +
			1);
	strcpy(G_RESOURCE_PATH, currWorkingDir);
	strcat(G_RESOURCE_PATH, &contentPath[0]);

	// Initialize default texture path
	G_DEFAULT_TEXTURE_PATH = malloc(strlen(defaultTexturePath) + 
			strlen(G_RESOURCE_PATH) + 1);
	strcpy(G_DEFAULT_TEXTURE_PATH, G_RESOURCE_PATH);
	strcat(G_DEFAULT_TEXTURE_PATH, defaultTexturePath);

	// Initialize default font path
	G_DEFAULT_FONT_PATH = malloc(strlen(defaultFontPath) + 
			strlen(G_RESOURCE_PATH) + 1);
	strcpy(G_DEFAULT_FONT_PATH, G_RESOURCE_PATH);
	strcat(G_DEFAULT_FONT_PATH, defaultFontPath);


	// Initialize Sprite Databases	
	Sprite_InitTextureDB(INITIAL_TEXTUREDB_SIZE);
	SpriteKaliDB_Init(1024);
	this->numLayers = numLayers;
	this->registryLayers = malloc(sizeof(SpriteRegistry *) * numLayers);
	for(i = 0; i < this->numLayers; i++) {
		this->registryLayers[i] = malloc(sizeof(SpriteRegistry));
		SpriteRegistry_Init(this->registryLayers[i], layerSize, i);
	}


	// Set up world
	World_Init(&this->world, WORLD_SIZE_X, WORLD_SIZE_Y, SECTION_SIZE,
			SPRITES_PER_SECTION);
	
	// Set the current camera to the bottom left of the world.
	this->camOffset.x = 0;
	this->camOffset.y = WORLD_SIZE_Y - WIN_SIZE_HEIGHT;



	// Set up world properties
	this->worldProps.gravity = 0.001;
	this->worldProps.friction = 0.001;



	// Initialize graphics
	this->windowWidth = WIN_SIZE_WIDTH;
	this->windowHeight = WIN_SIZE_HEIGHT;
	Graphics_Init(&this->graphics, WIN_SIZE_HEIGHT,
			WIN_SIZE_WIDTH, "Instance");
	Graphics_SetResizable(&this->graphics, 1);
	//G_SPRITE_DRAW_DIM_MULT_W = SPRITE_SCALE_BASE_WIDTH / this->windowWidth;
	//G_SPRITE_DRAW_DIM_MULT_H = SPRITE_SCALE_BASE_HEIGHT / this->windowHeight;




	// Initialize input
	InputHandler_Init(&this->input);



	// Add keyboard mappings	
	InputHandler_AddMouseMotionMapping(&this->input, MouseMotionHandler);
	InputHandler_AddMouseButtonMapping(&this->input, SDL_BUTTON_LEFT,
			INPUT_HANDLER_BUTTONDOWN, SpawnPlayer);
	InputHandler_AddKeyboardMapping(&this->input, SDL_SCANCODE_B,
			INPUT_HANDLER_KEYUP, SpawnRandomBall);

	InputHandler_AddKeyboardMapping(&this->input, SDL_SCANCODE_SPACE,
			INPUT_HANDLER_KEYUP, Input_Player_Jump);
	InputHandler_AddKeyboardMapping(&this->input, SDL_SCANCODE_D,
			INPUT_HANDLER_KEYDOWN, Input_Player_MoveRight);
	InputHandler_AddKeyboardMapping(&this->input, SDL_SCANCODE_D,
			INPUT_HANDLER_KEYUP, Input_Player_StopMoveRight);
	InputHandler_AddKeyboardMapping(&this->input, SDL_SCANCODE_A,
			INPUT_HANDLER_KEYDOWN, Input_Player_MoveLeft);
	InputHandler_AddKeyboardMapping(&this->input, SDL_SCANCODE_A,
			INPUT_HANDLER_KEYUP, Input_Player_StopMoveLeft);
	InputHandler_AddKeyboardMapping(&this->input, SDL_SCANCODE_O,
			INPUT_HANDLER_KEYUP, ToggleOverlay);
	
	// Seed rand
	srand(time(NULL));


	this->playerSpawned = 0;
	this->player = NULL;
	this->passedTicks = 0;
	return 0;
	

} // End Instance_Init




int Instance_Run(Instance *this, void *args) {
	int i;	
	Sprite *mouseCursor = 
		SpriteKaliDB_BuildSpriteByID(this->registryLayers[this->numLayers - 1], 
				1, &this->graphics);
	
	Sprite_AddUpdateFunc(mouseCursor, MouseCursor);
	//Sprite_AddUpdateFunc(mouseCursor, UpdateSpriteSections);	
	mouseCursor->_ignoreOffset = 1;
	Sprite_SetNoCollide(mouseCursor, 1);
	

	// Text testing
	PlainText text;
	PlainText_Init(&text, this->registryLayers[0], 10000, &this->graphics);
	PlainText_SetTextColor(&text, (SDL_Color) {255,255,255,0});
	PlainText_SetText(&text, "Hello World");
	PlainText_SetFont(&text, 
			"/home/aflores/programming/games/newGame/bin/content/fonts/font.ttf"
			,12);
	PlainText_RenderSolid(&text, &this->graphics);
	text._base._isVisible = 0;


	SDL_ShowCursor(SDL_DISABLE);



	// Create the ground for the world:
	Sprite *currSprite;
	int numSprites = this->world.xBoundary / 64;
	for(i = 0; i < numSprites; i++) {
		currSprite = SpriteKaliDB_BuildSpriteByID(this->registryLayers[0], 
				500, &this->graphics);
		Sprite_SetPos(currSprite, i * 64, this->world.yBoundary - 64);
		Sprite_AddUpdateFunc(currSprite, UpdateSpriteSections);
	}

	
	// Main game loop	
	uint32_t startTicks = 0;
	uint32_t delayedTicks = 0;
	uint32_t passedTicks = 0;
	uint32_t totalPassedTicks = 0;
	uint32_t timeSinceLastPrintedFrame = 0;
	int quit = 0;
	SDL_Event event;
	while(!quit) {
		startTicks = SDL_GetTicks();
		totalPassedTicks = delayedTicks + passedTicks;
		timeSinceLastPrintedFrame += totalPassedTicks;
		
		SDL_PumpEvents();	
		InputHandler_ProcessKeyInput(&this->input, this, 
				totalPassedTicks);

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				
				case(SDL_WINDOWEVENT) : 
					if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						this->windowWidth = event.window.data1;
						this->windowHeight = event.window.data2;
						/*
						G_SPRITE_DRAW_DIM_MULT_W = 
							SPRITE_SCALE_BASE_WIDTH / this->windowWidth;
						G_SPRITE_DRAW_DIM_MULT_H = 
							SPRITE_SCALE_BASE_HEIGHT / this->windowHeight;
						*/	
					}
					break;

				case(SDL_QUIT) :
					quit = 1;
					break;

				default :
					// Event gets passed by
					break;

			}
		}

							
		// All events handled, update the game
		
		// Update the player and the camera offset to the player's current pos 
		
		Graphics_Clear(&this->graphics); // Clear existing images

		if(this->player != NULL) {
			Sprite_ManualUpdate(this->player, totalPassedTicks, 
					this->player->_reg, this);
			this->camOffset.x = this->player->_boundingBox.x - 
				(this->windowWidth / 2);
			this->camOffset.y = this->player->_boundingBox.y - 
				(this->windowHeight / 2);
		}
		
		
		// Update overlay:

		if(this->player != NULL) {
			OverlayElement_Update((OverlayElement *) &this->overlay, 
					this->camOffset.x, this->camOffset.y);
			DebugOverlay_Update(&this->overlay, this->player);
		}


		// Update everything
		for(i = 0; i < this->numLayers; i++) {
			SpriteRegistry_UpdateAll(this->registryLayers[i], totalPassedTicks,
					this);
			SpriteRegistry_DrawAll(this->registryLayers[i], &this->graphics, 
					&this->camOffset);
		}



		
		// Check For Collisions:
		World_CheckForCollisions(&this->world); 

		if(timeSinceLastPrintedFrame > TICKS_PER_FRAME) {
			// Enough time has passed to print the frame to the screen.
			Graphics_Update(&this->graphics);
			timeSinceLastPrintedFrame = 0;
		}


		// Has enough time gone by to update the frame?
		passedTicks = SDL_GetTicks() - startTicks;
		if((INTERNAL_TICKS_PER_FRAME - ((int) passedTicks)) > 0) {
			delayedTicks = INTERNAL_TICKS_PER_FRAME - passedTicks;
			SDL_Delay(delayedTicks);
		}
#ifdef DEBUG_FRAMES
		printf("Frame took %u ticks\r\n", passedTicks);
		printf("Ticks waited between frames: %u\r\n", delayedTicks);
#endif	
	

	}	

	// Quit. Cleanup everything.
	
	TTF_Quit();
	SDL_Quit();

	return 0;


} // End Instance_Run


int Instance_Destroy(Instance *this) {
	int i;
	for(i = 0; i < this->numLayers; i++) {
		SpriteRegistry_Destroy(this->registryLayers[i]);
		free(this->registryLayers[i]);
	}
	Graphics_Destroy(&this->graphics);
	InputHandler_Destroy(&this->input);
	SpriteKaliDB_Destroy();
	World_Destroy(&this->world);
	free(G_RESOURCE_PATH);

	return 0;
}

// Input functions (to be mapped to a key or other input).

void * MouseMotionHandler(SDL_Event *e, void *instance) {
	Instance *inst = instance;
	/*	
	inst->mouseSprite->_boundingBox.x = e->motion.x - 
		(inst->mouseSprite->_boundingBox.w / 2);
	inst->mouseSprite->_boundingBox.y = e->motion.y -
		(inst->mouseSprite->_boundingBox.h / 2);
	*/
	inst->input._mouseY = e->motion.y;
	inst->input._mouseX = e->motion.x;
	return NULL;
}


void * SpawnRandomBall(SDL_Event *e, void *instance) {
	Instance *inst = instance;
	int timeOfDeath;
	char timeOfDeathLabel[] = "timeOfDeath";
	float maxXVel, maxYVel, minXVel, minYVel;
	BasicPhysProps *props;	
	
	maxXVel = 0.5f;
	maxYVel = 0.0f;
	minXVel = -0.5f;
	minYVel = -1.0f;


	PhysBall *newBall = (PhysBall *) SpriteKaliDB_BuildSpriteByID(
			inst->registryLayers[0], 100, &inst->graphics);
	

	Sprite_SetPos((Sprite *) newBall, inst->input._mouseX - 
			(newBall->_base._boundingBox.w / 2) + inst->camOffset.x,
			inst->input._mouseY - (newBall->_base._boundingBox.h / 2) + 
			inst->camOffset.y);

	props = &newBall->_base._props;
	newBall->_base._isVisible = 1;
	props->velX = RandomInt(minXVel, maxXVel);
	props->velY = RandomInt(minYVel, maxYVel);
	props->accX = RandomInt(-0.0005f, 0.0005f);
	props->accY = 0;
	
	Sprite_AddUpdateFunc((Sprite *) newBall, Disappear);
	Sprite_AddUpdateFunc((Sprite *) newBall, UpdateSpriteSections);

	timeOfDeath = (int) RandomInt(500.0f, 2000.0f);	
	Sprite_AddTag((Sprite *) newBall, timeOfDeathLabel, &timeOfDeath, 
			sizeof(int));

	return NULL;
}



void * OnClick(SDL_Event *e, void *instance) {	
		
	return NULL;
}



void * PanCamera(SDL_Event *e, void *instance) {
	Instance *inst = instance;
	inst->camOffset.x += 10;

	return NULL;
}



void * ToggleOverlay(SDL_Event *e, void *instance) {
	Instance *inst = instance;

	if(OverlayElement_IsVisible((OverlayElement *) &inst->overlay)) {
		Overlay_Hide(&inst->overlay);
	}
	else {
		Overlay_Show(&inst->overlay);
	}
		
	return NULL;
} // End ShowOverlay


// Player input functions

void * SpawnPlayer(SDL_Event *e, void *instance) {
	Instance *inst = instance;
	if(!inst->playerSpawned) {
		inst->playerSpawned = 1;
		inst->player = SpriteKaliDB_BuildSpriteByID(
				inst->registryLayers[inst->numLayers - 2], 2, &inst->graphics);
		Sprite_SetDimensions(inst->player, 32, 64);

		Sprite_SetPos(inst->player, inst->input._mouseX - 
				(inst->player->_boundingBox.w / 2) + inst->camOffset.x,
				inst->input._mouseY - (inst->player->_boundingBox.h / 2) + 
				inst->camOffset.y);

		Sprite_AddUpdateFunc(inst->player, UpdateSpriteSections);

		// Set up player debug overlay
		DebugOverlay_Init(&inst->overlay, inst->player, 
				inst->registryLayers[inst->numLayers - 1], &inst->graphics);

		/*
		Sprite_SetRotation(inst->player, 60.0f);
		Sprite_SetRotationCentered(inst->player, 1);
		Sprite_SetIsRotated(inst->player, 1);
		*/	
	}
	return NULL;
} // End SpawnPlayer


void * Input_Player_Jump(SDL_Event *e, void *instance) {
	Instance *inst = instance;

	Player_Jump((Player *) inst->player);
	return NULL;
}

void * Input_Player_MoveRight(SDL_Event *e, void *instance) {
	Instance *inst = instance;
	Player_MoveRight((Player *) inst->player, inst->passedTicks);
	return NULL;
}

void * Input_Player_StopMoveRight(SDL_Event *e, void *instance) {
	Instance *inst = instance;
	Player_StopMoveRight((Player *) inst->player);
	return NULL;
}

void * Input_Player_MoveLeft(SDL_Event *e, void *instance) {
	Instance *inst = instance;
	Player_MoveLeft((Player *) inst->player, inst->passedTicks);
	return NULL;
}

void * Input_Player_StopMoveLeft(SDL_Event *e, void *instance) {
	Instance *inst = instance;
	Player_StopMoveLeft((Player *) inst->player);
	return NULL;
}






// Update functions (for sprites)


void Disappear(void *thisPtr, uint32_t passedTicks, void *args) {
	PhysBall *this = thisPtr;
	//Instance *inst = args;
	int timeOfDeath;
	char timeOfDeathLabel[] = "timeOfDeath";
	
	Sprite_GetTagData((Sprite *) this, timeOfDeathLabel, &timeOfDeath,
			sizeof(int), NULL);

	if(this->_base._timeAlive >= timeOfDeath) {
		SpriteKaliDB_DestroySprite(thisPtr);
		free(this);
	}

} // End Disappear





void MouseCursor(void *thisPtr, uint32_t passedTicks, void *instance) {
	Sprite *this = thisPtr;
	Instance *inst = instance;

	this->_boundingBox.x = inst->input._mouseX - 
		(this->_boundingBox.w / 2) + inst->camOffset.x;
	this->_boundingBox.y = inst->input._mouseY -
		(this->_boundingBox.h / 2) + inst->camOffset.y;

} // End MouseCursor




void UpdateSpriteSections(void *thisPtr, uint32_t passedTicks, void *instance) {
	Sprite *this = thisPtr;	
	Instance *inst = (Instance *) instance;
	World_UpdateSections(&inst->world, this);
} // End UpdateSpriteSections







// Misc:

float RandomInt(float min, float max) {
	return ((((float) rand()) / RAND_MAX) * (max - min)) + min;
}




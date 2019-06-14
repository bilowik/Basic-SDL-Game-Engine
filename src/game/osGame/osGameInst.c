#include "game/osGame/osGameInst.h"
#include "engine/plainText.h"
#include "engine/overlay.h"
#include "engine/overlayElement.h"
#include "engine/overlayImage.h"
#include "game/sprites/wallpaper.h"
#include <time.h>

#define WIN_SIZE_WIDTH 800
#define WIN_SIZE_HEIGHT 600
#define FPS 60
#define INTERNAL_FPS 120
#define TICKS_PER_FRAME  1000 / FPS
#define INTERNAL_TICKS_PER_FRAME 1000 / INTERNAL_FPS
#define INITIAL_TEXTUREDB_SIZE 128
#define SMPENGINE_DEFAULT_NUM_LAYERS 12
#define SMPENGINE_DEFAULT_LAYER_SIZE 500
#define SPRITE_SCALE_BASE_WIDTH 1280
#define SPRITE_SCALE_BASE_HEIGHT 720


int OsGameInst_Init(OsGameInst *this, int argc, char **argv, int numLayers, 
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



	// Initialize graphics
	this->windowWidth = WIN_SIZE_WIDTH;
	this->windowHeight = WIN_SIZE_HEIGHT;
	Graphics_Init(&this->graphics, WIN_SIZE_HEIGHT,
			WIN_SIZE_WIDTH, "OsGameInst");




	// Initialize input
	InputHandler_Init(&this->input);
	InputHandler_AddMouseMotionMapping(&this->input, MouseMotionHandler);

	
	// Seed rand
	srand(time(NULL));


	// Set offset to 0
	this->camOffset = (SDL_Rect) {0, 0, 0, 0};

	this->passedTicks = 0;
	return 0;
	

} // End OsGameInst_Init




int OsGameInst_Run(OsGameInst *this, void *args) {
	int i;	
	Sprite *mouseCursor = 
		SpriteKaliDB_BuildSpriteByID(this->registryLayers[this->numLayers - 1], 
				501, &this->graphics);
	
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


	// Add a wallpaper
	Wallpaper wallpaper;
	Wallpaper_Init(&wallpaper, this->registryLayers[0], 101, &this->graphics);

	SDL_ShowCursor(SDL_DISABLE);




	
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

		


		// Update everything
		for(i = 0; i < this->numLayers; i++) {
			SpriteRegistry_UpdateAll(this->registryLayers[i], totalPassedTicks,
					this);
			SpriteRegistry_DrawAll(this->registryLayers[i], &this->graphics, 
					&this->camOffset);
		}



		

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
	OsGameInst_Destroy(this);
	TTF_Quit();
	SDL_Quit();

	return 0;


} // End OsGameInst_Run


int OsGameInst_Destroy(OsGameInst *this) {
	int i;
	for(i = 0; i < this->numLayers; i++) {
		SpriteRegistry_Destroy(this->registryLayers[i]);
		free(this->registryLayers[i]);
	}
	free(this->registryLayers);
	Graphics_Destroy(&this->graphics);
	InputHandler_Destroy(&this->input);
	SpriteKaliDB_Destroy();
	free(G_RESOURCE_PATH);

	return 0;
}

// Input functions (to be mapped to a key or other input).

void * MouseMotionHandler(SDL_Event *e, void *instance) {
	OsGameInst *inst = instance;
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




void MouseCursor(void *thisPtr, uint32_t passedTicks, void *instance) {
	Sprite *this = thisPtr;
	OsGameInst *inst = instance;

	this->_boundingBox.x = inst->input._mouseX + inst->camOffset.x;
	this->_boundingBox.y = inst->input._mouseY + inst->camOffset.y;

} // End MouseCursor




// Misc:

float RandomInt(float min, float max) {
	return ((((float) rand()) / RAND_MAX) * (max - min)) + min;
}




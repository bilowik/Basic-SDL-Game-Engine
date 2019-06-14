#include "game/test/debugOverlay.h"
#include "engine/overlayImage.h"
#include "engine/overlayElement.h"
#include "engine/plainText.h"
#include "engine/globals.h"

static const char PLAYER_POS[] = "Player pos: (%f, %f)";
static const char PLAYER_ONGROUND[] = "Player grounded %d";

static struct {
	OverlayImage *overlayImage;
	PlainText *line1;
	PlainText *line2;
	PlainText *line3;
	Graphics *graphicsPtr;
} debugOverlay;


void DebugOverlay_Init(Overlay *this, Sprite *player, 
		SpriteRegistry *overlayReg, Graphics *graphics) {
	float posX, posY;
	char buffer[256];
	OverlayImage *element;	
	PlainText *plainText;	
	


	debugOverlay.graphicsPtr = graphics;

	Sprite_GetPrecisePos(player, &posX, &posY);

	// Initialize overlay
	Overlay_Init(this, 200, 52, 2);


	// Set up an element a text sprite
	element = malloc(sizeof(OverlayImage));
	plainText = malloc(sizeof(PlainText));
	OverlayImage_Init(element, 100, 26, 5);
	PlainText_Init(plainText, overlayReg, 1000, graphics);
	debugOverlay.overlayImage = element;


	// Set the text and create the texture line 1
	sprintf(buffer, PLAYER_POS, posX, posY);
	PlainText_SetText(plainText, buffer);
	PlainText_SetFont(plainText, G_DEFAULT_FONT_PATH, 12);
	PlainText_SetTextColor(plainText, (SDL_Color) {255, 255, 255, 0});
	PlainText_RenderSolid(plainText, graphics);

	// Add to element:
	OverlayImage_AddSprite(element, (Sprite *) plainText, 0, 0);
	OverlayElement_SetVisibility((OverlayElement *) element, 1);


	// Add it to the debugOverlay struct:
	debugOverlay.line1 = plainText;

	// FOR LINE 2
	
	// Set up an element a text sprite
	plainText = malloc(sizeof(PlainText));
	PlainText_Init(plainText, overlayReg, 1000, graphics);

	// Set the text and create the texture for line 2
	sprintf(buffer, PLAYER_ONGROUND, player->_props.grounded);
	PlainText_SetText(plainText, buffer);
	PlainText_SetFont(plainText, G_DEFAULT_FONT_PATH, 12);
	PlainText_SetTextColor(plainText, (SDL_Color) {255, 255, 255, 0});
	PlainText_RenderSolid(plainText, graphics);
	
	// Add to element:
	OverlayImage_AddSprite(element, (Sprite *) plainText, 0, 13);
	OverlayElement_SetVisibility((OverlayElement *) element, 1);

	// Add element to overlay
	OverlayElement_ModifyOffset((OverlayElement *) element, 0, 0);
	Overlay_AddElement(this, (OverlayElement *) element);


	// Add line to debugOverlay
	debugOverlay.line2 = plainText;

} // End DebugOverlay_Init




void DebugOverlay_Update(Overlay *this, Sprite *player) {
	char buffer[256];
	float posX, posY;	
	Sprite_GetPrecisePos(player, &posX, &posY);

	sprintf(buffer, PLAYER_POS, posX, posY);
	PlainText_SetText(debugOverlay.line1, buffer);
	PlainText_RenderSolid(debugOverlay.line1, debugOverlay.graphicsPtr);
	
	
	sprintf(buffer, PLAYER_ONGROUND, player->_props.grounded);	
	PlainText_SetText(debugOverlay.line2, buffer);
	PlainText_RenderSolid(debugOverlay.line2, debugOverlay.graphicsPtr);

} // End DebugOverlay_Update





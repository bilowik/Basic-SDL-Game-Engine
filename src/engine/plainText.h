#ifndef SMPENGINE_PLAINTEXT_H
#define SMPENGINE_PLAINTEXT_H

#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include "graphics.h"

typedef struct PlainText PlainText;


struct PlainText {
	Sprite _base;
	char* _text;
	TTF_Font* _font;
	SDL_Color _textColor;
	SDL_Color _backgroundColor;
	

}; // End PlainText Class


// Constructors
void PlainText_Init(void *thisPtr, SpriteRegistry *reg, int id, 
		Graphics *graphics);

// Destructor
void PlainText_Destroy(void *thisPtr);



void PlainText_SetTextColor(PlainText* this, SDL_Color newColor);
// Purpose	:	Sets the color of future prined text
// Pre		:	'PlainText' has been initialized
// Post		:	Future text will be printed with the 
// 			new RGB values

void PlainText_SetBackgroundColor(PlainText* this, SDL_Color newColor);
// Sets the background color for future text printing

void PlainText_SetTextAlpha(PlainText* this, uint8_t newAlpha);
// Sets the alpha for future printed text

void PlainText_SetText(PlainText* this, const char* newText);
// Purpose	:	Sets the text of future printed text
// Pre		:	'PlainText' has been initialized
// Post		:	Future text will be printed with 
// 			'_text' as it's string.

void PlainText_SetFont(PlainText* this, const char* pathToFont, int fontSize);
// Purpose	:	Sets the font for the text
// Pre		:	'PlainText' is initialized.
// Post		:	The font has been set.

void PlainText_SetLocation(PlainText* this, int x, int y);
// Purpose	:	Sets the position where the text will be drawn
// Pre		:	'PlainText' has been initialized
// Post		:	'_boundingBox.x' and '_boundingBox.' are set.

void PlainText_GetTextureDimensions(PlainText* this, /*out*/ int* w,
		/*out*/ int* h);
// w and h are filled with the current texture's dimensions

SDL_Rect* PlainText_GetBoundingBoxPtr(PlainText* this);
// Returns a pointer to the '_boundingBox'

void PlainText_SetIsCentered(PlainText* this, uint8_t isCentered);
// Sets '_isCentered'	

void PlainText_RenderSolid(PlainText* this, Graphics* graphics);
// Purpose	:	Renders the current text to the screen
// Pre		:	'PlainText' is initialized
// 			A font has been set. ('setFont()')
// 			'_text' has been set. ('setText()')
// Post		:	The text is drawn to the renderer with
// 			the previously specified colors.
// 			'_boundingBox' is updated with the new size
// 			of the rendered texture.	

void PlainText_RenderShaded(PlainText* this, Graphics* graphics);
// Similar to 'renderSolid()' but is antialiased and has a background 
// with color '_backgroundColor'

void PlainText_RenderBlended(PlainText* this, Graphics* graphics);	
// Very high quality, no background but very slow (in comparison).

void PlainText_Draw(PlainText* this, Graphics* graphics);
// Purpose	:	Draws the rendered text to the renderer	
// Pre		:	'PlainText' is initialized
// 			render*() has been called to render text
// Post		:	The text has been copied to the renderer at the
// 			point defined at '_boundingBox'

#endif

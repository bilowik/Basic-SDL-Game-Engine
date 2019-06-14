#include "engine/sprite.h"
#include "engine/plainText.h"
#include "engine/spriteRegistry.h"
#include <string.h>
#include <SDL2/SDL.h>


void PlainText_Init(void *thisPtr, SpriteRegistry *reg, int id,
		Graphics *graphics) {
	PlainText *this = thisPtr;
	Sprite_Init(thisPtr, reg, NULL, graphics);

	this->_textColor = (SDL_Color) {0, 0, 0, 255};
	this->_backgroundColor = (SDL_Color) {255, 255, 255, 255};
	this->_text = NULL;
	this->_font = NULL;
	this->_base._isVisible = 1;



	((Sprite *) thisPtr)->_name = NULL;
	((Sprite *) thisPtr)->_partNames = NULL;
	((Sprite *) thisPtr)->_numParts = 1;
	((Sprite *) thisPtr)->_currPart = 0;
	((Sprite *) thisPtr)->_noCollide = 1;
	((Sprite *) thisPtr)->_sheet = NULL;


} // End PlainText_Init




void PlainText_Destroy(void* thisPtr) {
	PlainText *this = thisPtr;
	free(this->_text);
	SDL_DestroyTexture(this->_base._sheet);

	Sprite_ClearSections(thisPtr);
} // End PlainText_Destroy







void PlainText_SetTextColor(PlainText* this, SDL_Color newColor) {
	this->_textColor = newColor;
} // End _setTextColor




void PlainText_SetBackgroundColor(PlainText* this, SDL_Color newColor) {
	this->_backgroundColor = newColor;
} // End _setBackgroundColor




void PlainText_SetTextAlpha(PlainText* this, uint8_t newAlpha) {
	this->_textColor.a = newAlpha;
} // End _setTextAlpha




void PlainText_SetText(PlainText* this, const char* newText) {
	if(this->_text != NULL) {
		free(this->_text);
	}

	this->_text = malloc(strlen(newText) + 1); //+1 for null term
	strcpy(this->_text, newText);

} // End _setText




void PlainText_SetFont(PlainText* this, const char* pathToFont, int fontSize) {
	if(this->_font != NULL) {
		TTF_CloseFont(this->_font);
	}
	this->_font = TTF_OpenFont(pathToFont, fontSize);
	
	if(this->_font == NULL) {
		printf("ERROR: Could not open \"%s\"\n TTF_Error: %s",
				pathToFont, TTF_GetError());
	}
} // End _setFont



void PlainText_GetTextureDimensions(PlainText* this, int* w,
		int* h) {
	*h = ((Sprite *) this)->_partRectArray[0].h;
	*w = ((Sprite *) this)->_partRectArray[0].w;
} // End _getTextureDimensions
      







void PlainText_RenderSolid(PlainText* this, Graphics* graphics) {
	SDL_Surface* tempSurface = TTF_RenderText_Solid(this->_font,
			this->_text, this->_textColor);
	
	if(this->_base._sheet != NULL) {
		SDL_DestroyTexture(this->_base._sheet);
	}

	this->_base._sheet = SDL_CreateTextureFromSurface(graphics->_renderer,
			tempSurface);	
	((Sprite *) this)->_partRectArray[0].w = tempSurface->w;
	((Sprite *) this)->_partRectArray[0].h = tempSurface->w;
	((Sprite *) this)->_boundingBox.w = tempSurface->w;
	((Sprite *) this)->_boundingBox.h = tempSurface->h;
	SDL_FreeSurface(tempSurface);
} // End _renderSolid




void PlainText_RenderShaded(PlainText* this, Graphics* graphics) {
	SDL_Surface* tempSurface = TTF_RenderText_Shaded(this->_font,
			this->_text, this->_textColor, this->_backgroundColor);
	if(this->_base._sheet != NULL) {
		SDL_DestroyTexture(this->_base._sheet);
	}

	this->_base._sheet = SDL_CreateTextureFromSurface(graphics->_renderer,
			tempSurface);
	((Sprite *) this)->_partRectArray[0].w = tempSurface->w;
	((Sprite *) this)->_partRectArray[0].h = tempSurface->w;
	((Sprite *) this)->_boundingBox.w = tempSurface->w;
	((Sprite *) this)->_boundingBox.h = tempSurface->h;
	SDL_FreeSurface(tempSurface);

} // End _renderShaded




void PlainText_RenderBlended(PlainText* this, Graphics* graphics) {
	SDL_Surface* tempSurface = TTF_RenderText_Blended(this->_font,
			this->_text, this->_textColor);

	if(this->_base._sheet != NULL) {
		SDL_DestroyTexture(this->_base._sheet);
	}

	this->_base._sheet = SDL_CreateTextureFromSurface(graphics->_renderer,
			tempSurface);

	((Sprite *) this)->_partRectArray[0].w = tempSurface->w;
	((Sprite *) this)->_partRectArray[0].h = tempSurface->w;
	((Sprite *) this)->_boundingBox.w = tempSurface->w;
	((Sprite *) this)->_boundingBox.h = tempSurface->h;
	SDL_FreeSurface(tempSurface);
} // End _renderBlended




void PlainText_Draw(PlainText* this, Graphics* graphics) {
	if(this->_base._sheet == NULL) {
		printf("ERROR: No texture to print! (PlainText)\r\n");
	}
	else {
		SDL_RenderCopy(graphics->_renderer, this->_base._sheet,
				&this->_base._partRectArray[0], &this->_base._boundingBox);
	}

} // End _draw




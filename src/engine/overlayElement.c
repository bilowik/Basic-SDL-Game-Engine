#include "engine/overlayElement.h"
#include <stdlib.h>
static void _OverlayElement_Destroy(void *thisPtr);
static void _OverlayElement_SetVisibility(void *thisPtr, int visibility);








void OverlayElement_Init(OverlayElement *this, int width, int height) {
	this->_width = width;
	this->_height = height;

	this->_isVisible = 1;
	this->_elementID = -1;
	this->_groupID = -1;
	this->_offsetX = 0;
	this->_offsetY = 0;
	this->_Destructor = _OverlayElement_Destroy;
	this->_SetVisibility = _OverlayElement_SetVisibility;
	this->_Update = NULL;
} // End OverlayElement_Init



void OverlayElement_Destroy(OverlayElement *this) {
	this->_elementID = -1;
	this->_groupID = -1;
} // End OverlayElement_Destroy




void OverlayElement_ModifyOffset(OverlayElement *this, int newOffsetX,
		int newOffsetY) {
	this->_offsetX = newOffsetX;
	this->_offsetY = newOffsetY;
} // End OverlayElement_ModifyOffset




void OverlayElement_ModifyDimensions(OverlayElement *this, int newWidth,
		int newHeight) {
	this->_width = newWidth;
	this->_height = newHeight;

} // End OverlayElement_ModifyDimensions



void OverlayElement_SetVisibility(OverlayElement *this, int visibility) {
	this->_SetVisibility(this, visibility);
} // End OverlayElement_SetVisibility


void OverlayElement_Update(OverlayElement *this, int offsetX, int offsetY) {
	this->_Update(this, offsetX, offsetY);
} // End OverlayElement_Update




int OverlayElement_IsVisible(OverlayElement *this) {
	int returnVal = 0;
	if(this == NULL) {
		returnVal = 1;
	}
	else {

		if(this->_isVisible) {
			if(OverlayElement_IsVisible(this->_parent)) {
				returnVal = 1;
			}
		}
	}


	return returnVal;
} // End OverlayElement_IsVisible





// static function definitions


void _OverlayElement_Destroy(void *thisPtr) {
	OverlayElement_Destroy(thisPtr);
} //End _OverlayElement_Destroy




void _OverlayElement_SetVisibility(void *thisPtr, int visibility) {
	((OverlayElement *) thisPtr)->_isVisible = visibility;
} // End _OverlayElement_SetVisibility



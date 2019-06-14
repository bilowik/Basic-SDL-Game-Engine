#ifndef SMPENGINE_OVERLAY_ELEMENT_H
#define SMPENGINE_OVERLAY_ELEMENT_H
#include <stdint.h>

typedef struct OverlayElement OverlayElement;



struct OverlayElement {
	int _offsetX;
	int _offsetY;
	int _width;
	int _height;

	int _elementID;
	int _groupID;

	uint8_t _isVisible : 1;
	//  If the element is hidden and is visible, it is not rendered.
	//  If the element is invisible, and not hidden, it is not rendered.
	//  For the element to be rendered, it must be visible and not hidden.	

	OverlayElement *_parent;
	// If the overlayElement is within another overlay, it's ptr is set here.

	/* virtual */ void (*_SetVisibility)(void *thisPtr, int visibility);
	/* virtual */ void (*_Destructor)(void *thisPtr);
	/* virtual */ void (*_Update)(void *thisPtr, int xOffset, int yOffset);
	// This function adjusts the offset of the element based on the camera's
	// offset and adjusts the objects within the element to conform to this
	// offset.
	
	// These functions are used to show and hide the element.
};




void OverlayElement_Init(OverlayElement *this, int width, int height);

void OverlayElement_Destroy(OverlayElement *this);


void OverlayElement_ModifyOffset(OverlayElement *this, int newOffsetX,
		int newOffsetY);
// Sets the x and y offset

void OverlayElement_ModifyDimensions(OverlayElement *this, int newWidth,
		int newHeight);
// Sets the width and height

void OverlayElement_SetVisibility(OverlayElement *this, int visibility);
// Sets the visibility value, 0 = invis, 1 = vis

int OverlayElement_IsVisible(OverlayElement *this);
// Cehcks if the element is visible.
// If this element is visible, the parent is checked to ensure it's visible too,
// if the parent is also visible, then its parent is checked.
// if an element has no parent, then it's parent is considered visible
// and so on until all parents are checked as visible, or one parent is found
// to be invisible, in which case 0 is returned.

void OverlayElement_Update(OverlayElement *this, int offsetX, int offsetY);
// To be called to update the element. Performs basic update,
// checking if the element should be shown, and then calling the
// attached update method.


#endif

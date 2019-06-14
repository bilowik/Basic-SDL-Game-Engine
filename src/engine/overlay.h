#ifndef SMPENGINE_OVERLAY_H
#define SMPENGINE_OVERLAY_H
#include "engine/overlayElement.h"



typedef struct Overlay Overlay;


struct Overlay {
	OverlayElement _base;

	OverlayElement **_elements;
	int _numElements;	
	int _sizeOfElementArray;
};



void Overlay_Init(Overlay *this, int widht, int height, int initialMaxElements);

void Overlay_Destroy(Overlay *this);



int Overlay_AddElement(Overlay *this,
		OverlayElement *element);
// Purpose	:	Adds an element to the overlay, and returns it's index in the 
// 				overlay.
// Pre		:	Overlay is initialized.
// Post		:	The element is added to the overlay. 
// Return	:	-1 if the element already exists,
// 				0 if the element doesn't exist and is added.


int Overlay_FindElement(Overlay *this,
		OverlayElement *element, int *index);
// Purpose	:	Checks if a specific element already exists within the 
// 				overlay
// Pre		:	Overlay is initialized.
// Post		:	1 is returned if the element is found, 0 otherwise.
// 				if index is NOT NULL, then it gets the index of that element
// 				within the overlay.

int Overlay_RemoveElement(Overlay *this,
		OverlayElement *element);
// Purpose	:	Removes the specified element from the overlay
// Pre		:	Overlay is initialized.
// Post		:	The element is removed if it exists and 0 is returned.
// 				If the element doesn't exist, -1 is returned.

int Overlay_ModifyElementOffsetByIndex(Overlay *this,
		int index, int offsetX, int offsetY);
// Purpose 	:	Modifies the element at the specified index with the new
// 				offsets.

int Overlay_HideElementByIndex(Overlay *this, int index);
// Hides th element at the given index, if index is invalid, -1 is returned.

int Overlay_ShowElementByIndex(Overlay *this, int index);
// Shows the elemenet at the given idnex, if the index is invalid, -1 is 
// returne.d

void Overlay_Hide(Overlay *this);
// Purpose	:	Hides the overlay so it is not rendered to the screen.
// Pre		:	Overlay is initalized.
// Post		:	All elements are hidden.

void Overlay_Show(Overlay *this);
// Purpose	:	Shows the overlay so it is rendered to the screen.
// Pre		:	Overlay is initialzed.
// Post		:	The overlay is shown.

void Overlay_ShowAll(Overlay *this);
// Purpose	:	Shows the overlay as well as makes all it's elements visible.
// Pre		:	Overlay is initialized.
// Post		:	All elements are made visible as well as the overlay

void Overlay_HideAll(Overlay *this);
// Purpose	:	Hides the overlay as well as makes all it's elements invisible.
// Pre		:	Overlay is initialized.
// Post		:	All elements are made invisible as well as the overlay.

void Overlay_SetVisibilityOfAll(Overlay *this, int visibility);
// Purpose	:	Sets the visibility of the overlay and all elements
// Pre		:	Overlay is initialized.
// Post		:	The visibility of all elements and the overlay is set.

int Overlay_SetElementVisibility(Overlay *this, int indexOfElement, 
		int visibility);
// Purpose	:	Sets the visibility of an element at the specified index;
// Pre		:	Overlay is initialized.
// Post		:	The visibility of the element is set if the index is within
// 				bounds. If not, -1 is returned. 0 is returned upon success


#endif

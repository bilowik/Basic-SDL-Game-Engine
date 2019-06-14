#include "engine/overlay.h"
#include <stdlib.h>



static void _Overlay_Destroy(void *thisPtr);
static void _Overlay_Update(void *thisPtr, int offsetX, 
		int offsetY);
static void _Overlay_SetVisibility(void *thisPtr, int visibility);

void Overlay_Init(Overlay *this, int width, int height,
		int initialMaxElements) {
	OverlayElement_Init(&this->_base, width, height);	
	this->_base._isVisible = 0;
	this->_base._parent = NULL;	
	
	this->_numElements = 0;
	this->_sizeOfElementArray = initialMaxElements;

	this->_elements = malloc(sizeof(OverlayElement *) * initialMaxElements);
	
	((OverlayElement *) this)->_Destructor = _Overlay_Destroy;
	((OverlayElement *) this)->_Update = _Overlay_Update;
	((OverlayElement *) this)->_SetVisibility = _Overlay_SetVisibility;
} // End Overlay_Init



void Overlay_Destroy(Overlay *this) {
	OverlayElement_Destroy(&this->_base);

	int i;
	int numElements = this->_numElements;
	for(i = 0; i < numElements; i++) {
		OverlayElement_Destroy(this->_elements[i]);
	}

} // End Overlay_Destroy




int Overlay_AddElement(Overlay *this,
		OverlayElement *element) {
	int returnVal = 0;
	// Check if the element ptr array is full and expand if necessary:
	if(this->_numElements == this->_sizeOfElementArray) {
		// The array is full and needs to be expanded
		this->_sizeOfElementArray *= 2;
		this->_elements = realloc(this->_elements, this->_sizeOfElementArray);
	}
	
	if(Overlay_FindElement(this, element, NULL)) {
		// Element already exists within this overlay, cannot be added again
		returnVal = -1;
	}
	else {

		this->_elements[this->_numElements] = element;
		element->_parent = (OverlayElement *) this;
		this->_numElements++;
	}

	return returnVal;

} // End Overlay_AddElement




int Overlay_FindElement(Overlay *this,
		OverlayElement *element, int *index) {
	int i;
	int elementExists = 0;
	int numElements = this->_numElements;

	for(i = 0; i < numElements; i++) {
		if(element == this->_elements[i]) {
			elementExists = 1;
			if(index != NULL) {
				*index = i;
			}
		}
	}

	return elementExists;
} // End Overlay_FindElement




int Overlay_RemoveElement(Overlay *this, 
		OverlayElement *element) {
	int returnVal = 0;
	int index = -1;
	int elementFound = Overlay_FindElement(this, element, &index);

	if(elementFound) {
		this->_elements[index] = this->_elements[this->_numElements - 1];
		this->_numElements--;	
	}
	else {
		returnVal = -1;
	}

	return returnVal;
} // End Overlay_RemoveElement





int Overlay_ModifyElementOffsetByIndex(Overlay *this,
		int index, int offsetX, int offsetY) {
	int returnVal = 0;	
	if(index < this->_numElements) {
		this->_elements[index]->_offsetX = offsetX;
		this->_elements[index]->_offsetY = offsetY;
	}
	else {
		returnVal = -1;
	}

	return returnVal;
} // End Overlay_ModifyOffsetByIndex



void Overlay_Hide(Overlay *this) {
	OverlayElement_SetVisibility((void *) this, 0);
} // End Overlay_Hide




void Overlay_Show(Overlay *this) {
	OverlayElement_SetVisibility((void *) this, 1);
} // End Overlay_Hide



void Overlay_ShowAll(Overlay *this) {
	int numElements = this->_numElements;
	int i;

	OverlayElement_SetVisibility((OverlayElement *) this, 1);
	for(i = 0; i < numElements; i++) {
		OverlayElement_SetVisibility(this->_elements[i], 1);
	}
} // End Overlay_ShowAll




void Overlay_HideAll(Overlay *this) {
	int numElements = this->_numElements;
	int i;

	OverlayElement_SetVisibility((OverlayElement *) this, 0);
	for(i = 0; i < numElements; i++) {
		OverlayElement_SetVisibility(this->_elements[i], 0);
	}
} // End Overlay_ShowAll




void Overlay_SetVisibilityOfAll(Overlay *this, int visibility) {
	int numElements = this->_numElements;
	int i;

	OverlayElement_SetVisibility((OverlayElement *) this, visibility);
	for(i = 0; i < numElements; i++) {
		OverlayElement_SetVisibility(this->_elements[i], visibility);
	}

} // End Overlay_SetVisibilityOfAll



int Overlay_SetElementVisibility(Overlay *this, int indexOfElement,
		int visibility) {
	int returnVal = 0;
	if(indexOfElement >= this->_numElements) {
		// The index is out of the range of elements
		returnVal = -1;
	}
	else {
		OverlayElement_SetVisibility(this->_elements[indexOfElement],
				visibility);
	}

	return returnVal;
} // End Overlay_SetElementVisibility







// static function definitions

void _Overlay_Destroy(void *thisPtr) {
	Overlay_Destroy(thisPtr);
} // End _Overlay_Destory



void _Overlay_Update(void *thisPtr, int offsetX, int offsetY) {
	Overlay *this = thisPtr;	
	int i;
	int numElements = this->_numElements;
	OverlayElement *element;
	for(i = 0; i < numElements; i++) {
		element = this->_elements[i];
		OverlayElement_Update(element, 
				offsetX + ((OverlayElement *) thisPtr)->_offsetX, 
				offsetY + ((OverlayElement *) thisPtr)->_offsetY);
	}
} // End _Overlay_Update




void _Overlay_SetVisibility(void *thisPtr, int visibility) {
	((OverlayElement *) thisPtr)->_isVisible = visibility;
} // End _Overlay_SetVisibility

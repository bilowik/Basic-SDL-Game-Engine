#include "inputHandler.h"

#define INPUT_HANDLER_EVENT_BUF_SIZE 512



void InputHandler_Init(InputHandler* this) {
	
	Mapi_Init(&this->_keyboardActions, sizeof(KeyboardAction), 0);
	Mapi_Init(&this->_mouseButtonActions, sizeof(MouseButtonAction), 0);
	this->_mouseMotionAction = NULL;	
	this->_mouseX = 0;
	this->_mouseY = 0;
	this->_mouseXPrev = 0;
	this->_mouseYPrev = 0;
} // End InputHandler_Init




void InputHandler_Destroy(InputHandler* this) {
	Mapi_Destroy(&this->_keyboardActions);
} // End InputHandler_Destroy



int InputHandler_AddKeyboardMapping(InputHandler *this, int sdlKeyScancode,
		int actionType, void *(*actionFunc)(SDL_Event *e, void*)) {
	int returnVal = 1;
	
	KeyboardAction *currAction = Mapi_At(&this->_keyboardActions, 
			sdlKeyScancode);

	if(currAction == NULL) {
		// The key doesn't exist yet in the map, so it must be added
		// before continuing.
		KeyboardAction newAction;
		memset(&newAction.actions, 0, sizeof(newAction.actions));
		Mapi_Set(&this->_keyboardActions, sdlKeyScancode, &newAction);
		currAction = Mapi_At(&this->_keyboardActions, sdlKeyScancode);
	}
	
	if(currAction->actions[actionType] == NULL) {
		// The specific action for this key has not been set.
		currAction->actions[actionType] = actionFunc;
	
	}
	else {
		// The action has already been set. It must be deleted first.
		returnVal = 0;
	}

	return returnVal;

} // End InputHandler_AddAction



int InputHandler_RemoveKeyboardMapping(InputHandler *this, int sdlKeyScancode) {
	int returnVal = 1;

	if(Mapi_Count(&this->_keyboardActions, sdlKeyScancode) == 0) {
		// The key doesn't have a mapping.	
		returnVal = 0;
	}
	else {
		// The key has a mapping
		Mapi_Erase(&this->_keyboardActions, sdlKeyScancode);
	}

	return returnVal;
} // End InputHandler_RemoveMapping



int InputHandler_RemoveKeyboardAction(InputHandler *this, int sdlKeyScancode,
		int actionType) {
	int returnVal;
	KeyboardAction *currAction = Mapi_At(&this->_keyboardActions, 
			sdlKeyScancode);

	if(currAction == NULL) {
		// The key doesn't have a mapping.	
		returnVal = 0;
	}
	else {
		// The key has a mapping
		if(currAction->actions[actionType] == NULL) {
			// The key has mappings, but not for the specific
			// actionType
			returnVal = 0;
		}
		else {
			// The key has a mapping for the specific actiontype
			currAction->actions[actionType] = NULL;
			returnVal = 1;
		}
	}

	return returnVal;
} // End InputHandler_RemoveKeyboardAction



int InputHandler_IsKeyActionMappingSet(InputHandler *this, int sdlKeyScancode,
		int actionType) {
	int actionExists;	

	KeyboardAction *currAction = Mapi_At(&this->_keyboardActions, 
			sdlKeyScancode);
	
	if(currAction == NULL) {
		// The key doesn't exist.
		actionExists = 0;
	}

	else {
		// The key exists.
		if(currAction->actions[actionType] == NULL) {
			// But the action isn't set.
			actionExists = 0;
		}

		else {
			// The action does exist.
			actionExists = 1;
		}
	}
	
	return actionExists;

} // End InputHandler_IsKeyActionMappingSet




int InputHandler_IsKeyMappingSet(InputHandler *this, int sdlKeyScancode) {
	return (Mapi_At(&this->_keyboardActions, sdlKeyScancode) != NULL);
}




int InputHandler_ProcessKeyInput(InputHandler *this, void *gameInstance,
		uint32_t currTicks) {	
	SDL_Event inputEventBuf[INPUT_HANDLER_EVENT_BUF_SIZE];
	int numKeyEvents, numMouseButtonEvents, numMouseMotionEvents, i;
	KeyboardAction *currKeyAction;
	MouseButtonAction *currButtonAction;

	
	// First check for keyboard events:
	
	numKeyEvents = SDL_PeepEvents( 	&inputEventBuf[0],
					INPUT_HANDLER_EVENT_BUF_SIZE,
					SDL_GETEVENT,
					SDL_KEYDOWN,
					SDL_KEYUP
					);


	// Handle each individual keyboard event:
	int currKey, numRepeatKeys;
	numRepeatKeys = 0;
	for(i = 0; i < numKeyEvents; i++) {
		if(inputEventBuf[i].key.repeat) {
			// The key was held down long enough for it to repeat.
			// Event is ignored.
			numRepeatKeys++;
			continue;
		}

		currKey = inputEventBuf[i].key.keysym.scancode;	
		currKeyAction = Mapi_At(&this->_keyboardActions, currKey);
		
		
		if(currKeyAction != NULL) {
			// The key has a mapping 

			// Check for KEYUP and KEYDOWN actions:
			
			if(inputEventBuf[i].type == SDL_KEYUP) {
				// It was a KEYUP Event
				//
				if(currKeyAction->actions[INPUT_HANDLER_KEYUP] != 
						NULL) {
					// This key has a KEYUP action	
					currKeyAction->actions[INPUT_HANDLER_KEYUP](
							&inputEventBuf[i],
							gameInstance
							);
				}
				// else it doesn't have a KEYUP event
			}

			else {
				// It was a KEYDOWN event

				if(currKeyAction->actions[INPUT_HANDLER_KEYDOWN] != 
						NULL) {
					// THis key has a KEYDOWN action
					currKeyAction->
						actions[INPUT_HANDLER_KEYDOWN](
							&inputEventBuf[i],
							gameInstance
							);
				}
				// else it doesn't have a KEYDOWN action.
				
			}

		} // End if(..==NULL)
	
	} // End for(...)




	// Now handle mouse button events
	int currButton;
	numMouseButtonEvents = SDL_PeepEvents(&inputEventBuf[0],
								INPUT_HANDLER_EVENT_BUF_SIZE,
								SDL_GETEVENT,
								SDL_MOUSEBUTTONDOWN,
								SDL_MOUSEBUTTONUP
								);

	for(i = 0; i < numMouseButtonEvents; i++) {
		currButton = inputEventBuf[i].button.button;
		currButtonAction = Mapi_At(&this->_mouseButtonActions, currButton);

		if(currButtonAction != NULL) {
			// The key has a mapping 
			
			// Check for BUTTONUP and BUTTONDOWN actions:

			if(inputEventBuf[i].type == SDL_MOUSEBUTTONUP) {
				// It was a button up event

				if(currButtonAction->actions[INPUT_HANDLER_BUTTONUP] != NULL) {
					// This key has an action for button up
					currButtonAction->actions[INPUT_HANDLER_BUTTONUP](
							&inputEventBuf[i],
							gameInstance
							);

				}
				// else it doesn't have a buttonup event

			}

			else {
				// It was a keydown event

				if(currButtonAction->actions[INPUT_HANDLER_BUTTONDOWN] !=
						NULL) {
					// This button has a buttondown action
					currButtonAction->actions[INPUT_HANDLER_KEYDOWN](
							&inputEventBuf[i],
							gameInstance
							);
				}
				// else it doesn't have a buttondown action
				
			}

		}

	}


	// Now handle mouse motion events
	numMouseMotionEvents = SDL_PeepEvents(&inputEventBuf[0],
								INPUT_HANDLER_EVENT_BUF_SIZE,
								SDL_GETEVENT,
								SDL_MOUSEMOTION,
								SDL_MOUSEMOTION
								);
	if(this->_mouseMotionAction != NULL) {
		// There exists a function to handle motion events	
		for(i = 0; i < numMouseMotionEvents; i++) {
			this->_mouseMotionAction(&inputEventBuf[i], gameInstance); 
		}
	}


	



	
	return 0;	
				
} // End InputHandler_ProcessInput

			


int InputHandler_GetMousePosDiffX(InputHandler* this) {
	return this->_mouseX - this->_mouseXPrev;
} // End _getMousePosDiffX


int InputHandler_GetMousePosDiffY(InputHandler* this) {
	return this->_mouseY - this->_mouseYPrev;
} // End _getMousePosDiffY


void InputHandler_GetMousePosDiff(InputHandler* this, int* diffX, int* diffY) {
	*diffX = InputHandler_GetMousePosDiffX(this);
	*diffY = InputHandler_GetMousePosDiffY(this);
} // End _getMousePosDiff



void InputHandler_UpdateMousePos(InputHandler* this, int mouseXNew, 
		int mouseYNew) {	
	this->_mouseXPrev = this->_mouseX;
	this->_mouseYPrev = this->_mouseY;
	this->_mouseX = mouseXNew;
	this->_mouseY = mouseYNew;

} // End _updateMousePos



int InputHandler_AddMouseMotionMapping(InputHandler *this,
		void *(*actionFunc)(SDL_Event *e, void *)) {
	int returnVal;

	if(this->_mouseMotionAction == NULL) {
		// There is no current mapping for mouse movement
		this->_mouseMotionAction = actionFunc;
		returnVal = 1;	
	}
	else {
		// A mapping already exists, must be deleted first.
		returnVal = 0;
	}


	return returnVal;
} // End InputHandler_AddMouseMOtionMapping




int InputHandler_AddMouseButtonMapping(InputHandler *this, uint8_t mouseButton,
		int actionType, void *(*actionFunc)(SDL_Event *e, void *)) {
	int returnVal = 1;
	
	MouseButtonAction *currAction = Mapi_At(&this->_mouseButtonActions, 
			mouseButton);

	if(currAction == NULL) {
		// This button has no mappings yet.
		MouseButtonAction newAction;
		memset (&newAction, 0, sizeof(MouseButtonAction));
		Mapi_Set(&this->_mouseButtonActions, mouseButton, &newAction);	
		currAction = Mapi_At(&this->_mouseButtonActions, mouseButton);
	}

	if(currAction->actions[actionType] == NULL) {
		// The specific action for this button hasn't been set
		currAction->actions[actionType] = actionFunc;

	}
	else {
		// The action has already been set. It must be delteed first.
		returnVal = 0;
	}

	return returnVal;

} // End InputHandler_AddMouseButtonMapping



int InputHandler_RemoveMouseButtonActionMapping(InputHandler *this,
		int sdlButtonCode, int actionType) { 
	int returnVal;
	MouseButtonAction *currAction = Mapi_At(&this->_mouseButtonActions, 
			sdlButtonCode);

	if(currAction == NULL) {
		// The key doesn't have a mapping.	
		returnVal = 0;
	}
	else {
		// The key has a mapping
		if(currAction->actions[actionType] == NULL) {
			// The key has mappings, but not for the specific
			// actionType
			returnVal = 0;
		}
		else {
			// The key has a mapping for the specific actiontype
			currAction->actions[actionType] = NULL;
			returnVal = 1;
		}
	}

	return returnVal;
} // End InputHandler_RemoveMouseButtonMapping




int InputHandler_IsMouseButtonActionMappingSet(InputHandler *this, 
		int sdlButtonCode, int actionType) {
	int actionExists;	

	MouseButtonAction *currAction = Mapi_At(&this->_mouseButtonActions, 
			sdlButtonCode);
	
	if(currAction == NULL) {
		// The key doesn't exist.
		actionExists = 0;
	}

	else {
		// The key exists.
		if(currAction->actions[actionType] == NULL) {
			// But the action isn't set.
			actionExists = 0;
		}

		else {
			// The action does exist.
			actionExists = 1;
		}
	}
	
	return actionExists;

} // End InputHandler_IsMouseButtonActionMappingSet




int InputHandler_IsMouseButtonMappingSet(InputHandler *this,
		int sdlButtonCode) {
	return (Mapi_At(&this->_mouseButtonActions, sdlButtonCode) != NULL);
} // End InputHandler_IsMouseButtonMappingSet



int InputHandler_IsMouseMotionMappingSet(InputHandler *this) {
	return (this->_mouseMotionAction != NULL);
}

/*
void InputHandler_StartNewFrame(InputHandler* this) {
	Keymap_Clear(&this->_pressedKeys);
	Keymap_Clear(&this->_releasedKeys);

} // End _startNewFrame


void InputHandler_KeyUpEvent(InputHandler* this, SDL_Event* event) {
	Keymap_ModifyKeyStatus(&this->_heldKeys,
		       	event->key.keysym.sym, 0);
	Keymap_ModifyKeyStatus(&this->_releasedKeys, 
			event->key.keysym.sym, 1);
} // End _keyUpEvent



void InputHandler_KeyDownEvent(InputHandler* this, SDL_Event* event) {
	Keymap_ModifyKeyStatus(&this->_pressedKeys, 
			event->key.keysym.sym, 1);
	Keymap_ModifyKeyStatus(&this->_heldKeys, 
			event->key.keysym.sym, 1);

} // End _keyDownEvent



int InputHandler_WasKeyPressed(InputHandler* this, SDL_Scancode key) {
	return Keymap_GetStatus(&this->_pressedKeys, key);
}

int InputHandler_IsKeyHeld(InputHandler* this, SDL_Scancode key) {
	return Keymap_GetStatus(&this->_heldKeys, key);
}

int InputHandler_WasKeyReleased(InputHandler* this, SDL_Scancode key) {
	return Keymap_GetStatus(&this->_releasedKeys, key);
}

*/

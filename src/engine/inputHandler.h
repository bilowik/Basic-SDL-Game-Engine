#ifndef SMPENG_INPUTHANDLER
#define SMPENG_INPUTHANDLER

#include <SDL2/SDL.h>
//#include "keymap/keymap.h" 
#include "vector/vector.h"
#include "mapi/mapi.h"
#include <string.h>

typedef struct InputHandler InputHandler;
typedef struct KeyboardAction KeyboardAction;
typedef struct MouseButtonAction MouseButtonAction;
enum KeyboardActionType {	INPUT_HANDLER_KEYDOWN,
							INPUT_HANDLER_KEYUP,
						};
enum MouseActionType {
						INPUT_HANDLER_BUTTONDOWN,
						INPUT_HANDLER_BUTTONUP
					};

struct KeyboardAction {
	// Each one of the KeyboardActionTypes corresponds to an action
	// in the funciton pointer array "actions". 
	// When a key event is polled and that specific key has been mapped,
	// this struct is checked to see if the key event triggers an action.
	
	void *(*actions[2])(SDL_Event *, void*);
	// Each index corresponds to a KeyboardActionType
	
	// This is a struct due to possible future additions	
};

struct MouseButtonAction {
	void *(*actions[2])(SDL_Event *, void *);
	// Same function as KeyboardAction but for mouse buttons.
};

struct InputHandler {
		
	Mapi _keyboardActions; // Maps a key to a function
	Mapi _mouseButtonActions; // Maps a mouse button to a function
	void * (*_mouseMotionAction)(SDL_Event *, void *instance);
	// Maps mouse motion to a function.

	// Private Fields	
	int _mouseY;
	int _mouseX;
	int _mouseYPrev;
	int _mouseXPrev;
	
	/*
	Keymap _pressedKeys;
	Keymap _heldKeys;
	Keymap _releasedKeys;
	*/
	
}; // End InputHandler



void InputHandler_Init(InputHandler* this);

void InputHandler_Destroy(InputHandler* this);




int InputHandler_AddKeyboardMapping(InputHandler *this, int sdlKeyScancode, 
		int actionType, void *(*actionFunc)(SDL_Event *, void *));
// Purpose	:	Adds the actionFunc to actionMap, mapping the scancode 
// 			to the function.
// Pre		:	InputHandler was initialized.
// Post		:	The function is added to the map, with the scancdoe
// 			being it's corresponding key. If an action already
// 			exists with a given key and type, then it isn't added.
// Args		:	"timing" is only required if it is of actionType HOLD
// 			or DOUBLETAP. 
// Return	:	Returns 1 upon success.
// 		:	Returns 0 if a actionFunc with the same key and type 
// 			already exists.


int InputHandler_RemoveKeyboardMapping(InputHandler *this, int sdlKeyScancode);
// Removes all mappings of a given key.


int InputHandler_ProcessKeyInput(InputHandler *this, void *gameInstance, 
		uint32_t currTicks);
// Purpose	:	Polls for input and then calls the proper functions
// Pre		:	InputHandler is initialized.
// 			GameInstance is not NULL
// Post		:	All mouse and keyboard input events are processed and
// 			removed from the sdl event queue. 


int InputHandler_AddMouseButtonMapping(InputHandler *this, uint8_t mouseButton, 
		int actionType, void *(*actionFunc)(SDL_Event *e, void *));
// Purpose	:	Maps a function to a mouse button
// Pre		:	InpuHandler is initialized,
// 			actionFunc is not NULL
// Post		:	actionFunc is now mapped to the specified mouse button.
// 				


int InputHandler_AddMouseMotionMapping(InputHandler *this,
		void *(*actionFunc)(SDL_Event *e, void *));
// Purpose	:	Maps a function to mouse motion.
// Pre		:	InputHandler is initialized,
// 			actionFunc is not NULL
// Post		:	actionFunc is now mapped to mouse motion.



int InputHandler_RemoveMouseButtonActionMapping(InputHandler *this,
		int sdlButtonCode, int actionType);
// Purpose	:	Removes a button action from the map.
// Pre		:	InputHandler is initialized
// Post		:	If the sdlButtoncode existed, then it and it's
// 			mapped function are removed.
// Return	:	1 upon successful removal.
// 			0 if the key didn't exist.



int InputHandler_RemoveMouseMotionMapping(InputHandler *this);
// Purpose	:	Removes the mouse motion mapping.
// Pre		:	InputHandler is initialized
// Post		:	The mouse motion function is now NULL.
// Return	:	1 upon successful removal.
// 			0 if the key didn't exist.



int InputHandler_IsMouseButtonActionMappingSet(InputHandler *this,
		int sdlKeyScancode, int actionType);
// Purpose	:	Checks if a specific actionType exists for a key
// Pre		:	InputHandler is initialized.
// Post		:	Returns 1 if it exists, 0 if not.


int InputHandler_IsMouseButtonMappingSet(InputHandler *this, int sdlKeyScancode);
// Purpose	:	Checks if a specific key has any mapping
// Pre		:	InputHandler is initialized
// Post		:	Returns 1 if it exists, 0 if not. 

int InputHandler_GetMousePosDiffX(InputHandler* this);
// Purpose	:	Returns the difference of '_mousePrevX' and
//  			'_mouseX'
// Pre		:	InputHandler has been initialized by a // Post		:	The difference is returned


int InputHandler_GetMousePosDiffY(InputHandler* this);
// SEE getMousePosDiffX(InputHandler* this)
// Y instead of X


void InputHandler_GetMousePosDiff(InputHandler* this, /*out*/int* diffX,
		/*out*/int* diffY);	
// Purpose	:	Returns the differences of the previous
// 			mouse coordinates and the current mouse
// 			coordinates in 'diffX' and 'diffY'
// Pre		:	InputHandler has been initialized by a 
// 			constructor 
// Post		:	'diffX' gets mouseX


void InputHandler_UpdateMousePos(InputHandler* this, int mouseXNew,
		int mouseYNew);
// Purpose	:	Updates current mouse coordinates.
// Pre		:	InputHandler has been initialized by a 
// 			constructor
// Post		:	'_mouseXPrev' is set to '_mouseX' and 
// 			'_mouseYPrev' is set to '_mouseY'
// 			'_mouseX' and '_mouseY' are updated with 
// 			'mouseXNew' and 'mouseYNew' respectively. 	

/*
void InputHandler_StartNewFrame(InputHandler* this);
// Purpose	:	Clears '_pressedKeys' and '_releasedKeys' for
// 			the new frame.
// Pre		:	InputHandler has been initialized by a
// 			constructor. 
// Post		:	'_pressedKeys' and '_releasedKeys' are 
// 			completely cleared.


void InputHandler_KeyUpEvent(InputHandler* this, SDL_Event* event);
// Purpose	:	Processes a key being released.
// Pre		:	InputHandler is initialized by a 
// 			constructor
// Post		:	'key' is removed from '_heldKeys' and 
// 			'_pressedKeys' and added to
// 			'_releasedKeys'


void InputHandler_KeyDownEvent(InputHandler* this, SDL_Event* event);
// Purpose	:	Processes a key being pressed.
// Pre		:	InputHandler is initialized by a 
// 			constructor
// Post		:	'key' is added to '_heldKeys' and 
// 			'_pressedKeys' 


int InputHandler_WasKeyPressed(InputHandler* this, SDL_Scancode key);
// Purpose	:	Checks if a certain 'key' was pressed
// 			this frame.
// Pre		:	InputHandler has been initialized by
// 			a constructor
// Post		:	Returns 1 if the 'key' is currently pressed
// 			Returns 0 if the 'key' is not currently
// 			pressed.


int InputHandler_IsKeyHeld(InputHandler* this, SDL_Scancode key);
// Purpose	:	Checks if a certain 'key' is held this frame.
// Pre		:	InputHandler is initialized by a constructor.
// Post		:	Returns 1 if the 'key' is currently held.
// 			Returns 0 if the 'key' is not currently held.


int InputHandler_WasKeyReleased(InputHandler* this, SDL_Scancode key);
// Purpose	:	Checks if a certain 'key' was just released
// 			this frame.
// Pre		:	InputHandler is initialized by a cnstructor
// Post		:	Returns 1 if the 'key' was released.
// 			Returns 0 if the 'key' wasn't released.
*/







#endif

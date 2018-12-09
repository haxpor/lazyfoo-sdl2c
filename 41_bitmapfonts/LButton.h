/*
 * LButton
 * Represents a *logical* button with 4 possible states of visual which are
 * 	- mouse out
 * 	- mouse over
 * 	- mouse down
 * 	- mouse up
 *
 * 	It works with LTexture which will represent its visual.
 */

#ifndef LButton_h_
#define LButton_h_

#include "SDL.h"

typedef enum 
{
	L_BUTTON_MOUSE_OUT = 0,
	L_BUTTON_MOUSE_OVER_MOTION = 1,
	L_BUTTON_MOUSE_DOWN = 2,
	L_BUTTON_MOUSE_UP = 3,
	L_BUTTON_TOTAL = 4
} LButtonState;

typedef struct
{
	LButtonState state;
} LButton;

/*
 * Create a new LButton then return its pointer via out pointer.
 * 
 * Return newly created LButton in which caller needs to free it with LButton_Free().
 */
extern LButton* LButton_Create();

/*
 * Call this function to update LButton's state, and execute user's custon handler function as set via its structure.
 *
 * User calls this in application's event loop.
 * Parameter
 * 	- button, LButton* : pointer to LButton
 * 	- e, SDL_Event* : pointer to SDL_Event as received in user's event loop code
 * 	- pos, SDL_Point : position that this button will be drawed
 * 	- buttonRect, SDL_Rect : rectangle defined the coordinate to render such LButton
 */
extern void LButton_HandleEvent(LButton* button, SDL_Event* e, SDL_Rect buttonRect);

/*
 * Free LButton.
 * After calling this function, button will be nil.
 * button cannot be nil.
 *
 * Paramter
 * 	- button LButton* : pointer to LButton to free
 */
extern void LButton_Free(LButton* button);

#endif 	/* LButton_h_ */


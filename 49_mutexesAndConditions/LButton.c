#include "LButton.h"
#include <stdbool.h>

LButton* LButton_Create()
{
  LButton* ptr = malloc(sizeof(LButton));
  ptr->state = L_BUTTON_MOUSE_OUT;

  return ptr;
}

void LButton_HandleEvent(LButton* button, SDL_Event* e, SDL_Rect buttonRect)
{
  // check for mouse event
  if (e->type == SDL_MOUSEMOTION ||
      e->type == SDL_MOUSEBUTTONDOWN ||
      e->type == SDL_MOUSEBUTTONUP)
  {
    // get mouse's position
    int x, y;
    SDL_GetMouseState(&x, &y);

    // check if mouse is in button
    bool inside = false;

    if (x >= buttonRect.x &&
        x <= buttonRect.x + buttonRect.w &&
        y >= buttonRect.y &&
        y <= buttonRect.y + buttonRect.h)
    {
      inside = true;
    }

    // mouse is outside button
    if (!inside)
    {
      button->state = L_BUTTON_MOUSE_OUT;	
    }
    // mouse is inside button
    else {
      // set mouse state
      switch (e->type)
      {
        case SDL_MOUSEMOTION:
          button->state = L_BUTTON_MOUSE_OVER_MOTION;
          break;	
        case SDL_MOUSEBUTTONDOWN:
          button->state = L_BUTTON_MOUSE_DOWN;
          break;
        case SDL_MOUSEBUTTONUP:
          button->state = L_BUTTON_MOUSE_UP;
          break;
      }
    }
  }
}

void LButton_Free(LButton* ptr)
{
  free(ptr);
  ptr = NULL;
}

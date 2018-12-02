#include "LWindow.h"
#include <stdlib.h>

#define WINDOW_TITLE_BUFFER 60
static char temp_chrs[WINDOW_TITLE_BUFFER];
static void initialize_values(LWindow* window)
{
  window->window = NULL;
  window->has_mouse_focus = false;
  window->has_keyboard_focus = false;
  window->is_minimized = false;
  window->_fullscreen = false;
  window->width = 0;
  window->height = 0;
  memset(window->original_title, 0, ORIGINAL_WINDOW_TITLE_BUFFER);
  window->on_window_resize = NULL;
}

LWindow* LWindow_new(const char* title, int screen_width, int screen_height, int flags)
{
  // allocate memory
  LWindow* out = malloc(sizeof(LWindow));
  
  // initially set values
  initialize_values(out);

  // initialize
  LWindow_init(out, title, screen_width, screen_height, flags);
  return out;
}

void LWindow_init(LWindow *window, const char* title, int screen_width, int screen_height, int flags)
{
  // always use SDL_WINDOW_SHOWN to logical or with input flags
  window->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN | flags);
  if (window->window != NULL)
  {
    window->has_mouse_focus = true;
    window->has_keyboard_focus = true;
    window->width = screen_width;
    window->height = screen_height;
    strncpy(window->original_title, title, ORIGINAL_WINDOW_TITLE_BUFFER);
  }
}

void LWindow_handle_event(LWindow* window, SDL_Event *e, SDL_Renderer* renderer)
{
  // window event occurred
  if (e->type == SDL_WINDOWEVENT)
  {
    // caption update flag
    bool update_caption = false;

    switch (e->window.event)
    {
      // get new dimensions and repaint on window size change
      case SDL_WINDOWEVENT_SIZE_CHANGED:
        window->width = e->window.data1;
        window->height = e->window.data2;

        // notify event via function pointer (if it's set)
        if (window->on_window_resize != NULL)
        {
          window->on_window_resize(window->width, window->height);
        }

        SDL_RenderPresent(renderer);
        break;

      // repaint on exposure
      case SDL_WINDOWEVENT_EXPOSED:
        SDL_RenderPresent(renderer);
        break;

      // mouse entered window
      case SDL_WINDOWEVENT_ENTER:
        window->has_mouse_focus = true;
        update_caption = true;
        break;

      // mouse left window
      case SDL_WINDOWEVENT_LEAVE:
        window->has_mouse_focus = false;
        update_caption = true;
        break;

      // window has keyboard focus
      case SDL_WINDOWEVENT_FOCUS_GAINED:
        window->has_keyboard_focus = true;
        update_caption = true;
        break;

      // window lost keyboard focus
      case SDL_WINDOWEVENT_FOCUS_LOST:
        window->has_keyboard_focus = false;
        update_caption = true;
        break;

      // window minimized
      case SDL_WINDOWEVENT_MINIMIZED:
        window->is_minimized = true;
        break;

      // window maximized
      case SDL_WINDOWEVENT_MAXIMIZED:
        window->is_minimized = false;
        break;

      // windnow restored
      case SDL_WINDOWEVENT_RESTORED:
        window->is_minimized = false;
        break;
    }

    // update window caption with new data
    if (update_caption)
    {
      // update window's title depending on focus of mouse and keyboard
      snprintf(temp_chrs, WINDOW_TITLE_BUFFER, "%s [%d|%d]", window->original_title, window->has_mouse_focus, window->has_keyboard_focus);
      SDL_SetWindowTitle(window->window, temp_chrs);
    }
  }
  // toggle fullscreen via enter key
  else if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_RETURN)
  {
    if (window->_fullscreen)
    {
      SDL_SetWindowFullscreen(window->window, SDL_FALSE);
      window->_fullscreen = false;
    }
    else
    {
      SDL_SetWindowFullscreen(window->window, SDL_TRUE);
      window->_fullscreen = true;
      window->is_minimized = false;
    }
  }
}

void LWindow_free_internals(LWindow* window)
{
  if (window->window != NULL)
  {
    SDL_DestroyWindow(window->window);
    window->window =  NULL;
  }
}

void LWindow_free(LWindow* window)
{
  // firstly free its internals
  LWindow_free_internals(window);
  // free its allocated space
  free(window);
  window = NULL;
}

#include "LWindow.h"
#include <stdlib.h>

#define WINDOW_TITLE_BUFFER 60
static char temp_chrs[WINDOW_TITLE_BUFFER];
static void initialize_values(LWindow* window)
{
  window->window = NULL;
  window->renderer = NULL;
  window->has_mouse_focus = false;
  window->has_keyboard_focus = false;
  window->is_minimized = false;
  window->is_shown = false;
  window->_fullscreen = false;
  window->width = 0;
  window->height = 0;
  memset(window->original_title, 0, ORIGINAL_WINDOW_TITLE_BUFFER);
  window->on_window_resize = NULL;
  window->handle_event = NULL;
  window->render = NULL;
}

LWindow* LWindow_new(const char* title, int screen_width, int screen_height, int window_flags, int renderer_flags)
{
  // allocate memory
  LWindow* out = malloc(sizeof(LWindow));
  
  // initially set values
  initialize_values(out);

  // initialize
  if(!LWindow_init(out, title, screen_width, screen_height, window_flags, renderer_flags))
  {
    // if failed to init, then free allocated memory
    LWindow_free(out);
    return NULL;
  }
  else
  {
    return out;
  }
}

bool LWindow_init(LWindow *window, const char* title, int screen_width, int screen_height, int window_flags, int renderer_flags)
{
  // always use SDL_WINDOW_SHOWN to logical or with input flags
  window->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN | window_flags);
  if (window->window != NULL)
  {
    window->has_mouse_focus = true;
    window->has_keyboard_focus = true;
    window->width = screen_width;
    window->height = screen_height;
    strncpy(window->original_title, title, ORIGINAL_WINDOW_TITLE_BUFFER);

    // check if there is opengl bit set then we don't create renderer
    if (!(window_flags & SDL_WINDOW_OPENGL))
    {
      // create renderer associates with this window
      // default use hardware acceleration
      window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED | renderer_flags);
      // if cannot create renderer, then we free window immediately
      if (window->renderer == NULL)
      {
        // free internals
        LWindow_free_internals(window);
        return false;
      }
    }
    
    // grab window identifier
    window->id = SDL_GetWindowID(window->window);
    // grab display index that this window is on
    window->display_id = SDL_GetWindowDisplayIndex(window->window);
    // set this window as opened
    window->is_shown = true;
  }

  // return result of initialization
  return true;
}

void LWindow_handle_event(LWindow* window, SDL_Event *e, float delta_time)
{
  // window event occurred
  if (e->type == SDL_WINDOWEVENT && e->window.windowID == window->id)
  {
    // caption update flag
    bool update_caption = false;

    switch (e->window.event)
    {
      // window moved
      case SDL_WINDOWEVENT_MOVED:
        window->display_id = SDL_GetWindowDisplayIndex(window->window);
        break;

      // window appeared
      case SDL_WINDOWEVENT_SHOWN:
        window->is_shown = true;
        break;
        
      // window disappeared
      case SDL_WINDOWEVENT_HIDDEN:
        window->is_shown = false;
        break;

      // get new dimensions and repaint on window size change
      case SDL_WINDOWEVENT_SIZE_CHANGED:
        window->width = e->window.data1;
        window->height = e->window.data2;

        // notify event via function pointer (if it's set)
        if (window->on_window_resize != NULL)
        {
          window->on_window_resize(window->id, window->width, window->height);
        }

        SDL_RenderPresent(window->renderer);
        break;

      // repaint on exposure
      case SDL_WINDOWEVENT_EXPOSED:
        SDL_RenderPresent(window->renderer);
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

      // hide on close
      case SDL_WINDOWEVENT_CLOSE:
        SDL_HideWindow(window->window);
        break;
    }

    // update window caption with new data
    if (update_caption)
    {
      // update window's title depending on focus of mouse and keyboard
      snprintf(temp_chrs, WINDOW_TITLE_BUFFER, "%s ID:%d [%d|%d]", window->original_title, window->id, window->has_mouse_focus, window->has_keyboard_focus);
      SDL_SetWindowTitle(window->window, temp_chrs);
    }
  }

  // execute user's code on handling event if set
  if (window->handle_event != NULL)
  {
    window->handle_event(e, delta_time);
  }
}

void LWindow_focus(LWindow* window)
{
  // restore window if needed
  if (window->is_shown)
  {
    SDL_ShowWindow(window->window);
  }

  // move window forward
  SDL_RaiseWindow(window->window);
}

void LWindow_free_internals(LWindow* window)
{
  // free associated renderer first
  if (window->renderer != NULL)
  {
    SDL_DestroyRenderer(window->renderer);
    window->renderer = NULL;
  }

  // free window next
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

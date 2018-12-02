#ifndef LWindow_h_
#define LWindow_h_

#include "SDL.h"
#include <stdbool.h>

#define ORIGINAL_WINDOW_TITLE_BUFFER 50

///
/// Wrapper window managmenet for SDL_Window
/// 
typedef struct {
  /// window
  SDL_Window *window;

  /// window width (read-only)
  int width;

  /// window height (read_only)
  int height;

  /// whether or not has mouse focus.
  /// internally managed, (read-only)
  bool has_mouse_focus;

  /// whether or not has keyboard focus
  /// internally managed (read-only)
  bool has_keyboard_focus;

  /// whether or not the window is minimized
  /// internally managed (read-only
  bool is_minimized;

  /// original title of window
  char original_title[ORIGINAL_WINDOW_TITLE_BUFFER];

  /// flag whether it's full screen or not, internally managed.
  bool _fullscreen;

  /// callback available to be set when window's size has changed
  /// This will be called before repainting occurs.
  /// \param new_width New window's width
  /// \param new_height New window/s height
  void (*on_window_resize)(int new_width, int new_height);
} LWindow;

///
/// Create a new LWindow.
/// Use LWindow_free() to free LWindow later.
/// \return Newly created LWindow instance on heap.
///
extern LWindow* LWindow_new(const char* title, int screen_width, int screen_height, int flags);

///
/// Initialize allocated LWindow.
/// Use LWindow_free_internals() to free LWindow created via this function.
/// \param [in,out] window Window instance
/// 
extern void LWindow_init(LWindow *window, const char* title, int screen_width, int screen_height, int flags);

///
/// Handle event associated with window.
/// \param window Window instance
/// \param e SDL_Event to handle
/// \param renderer SDL_Renderer. Mostly will be used when it need to repaint the window.
///
extern void LWindow_handle_event(LWindow *window, SDL_Event *e, SDL_Renderer* renderer);

///
/// Free internals of LWindow.
/// Use this to free memory created by LWindow via LWindow_init().
///
extern void LWindow_free_internals(LWindow *window);

/// Free memory of LWindow.
/// Use this to free memory created by LWindow via LWindow_new().
///
extern void LWindow_free(LWindow *window);

#endif

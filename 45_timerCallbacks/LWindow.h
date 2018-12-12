#ifndef LWindow_h_
#define LWindow_h_

#include "SDL.h"
#include <stdbool.h>

#define ORIGINAL_WINDOW_TITLE_BUFFER 50

///
/// Wrapper window managmenet for SDL_Window
/// 
typedef struct {
  /// id represents this window
  Uint32 id;
  
  // display id that this window is on
  int display_id;

  /// window
  SDL_Window* window;

  /// renderer associates with this window
  SDL_Renderer* renderer;

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

  /// whether the window is shown
  bool is_shown;

  /// original title of window
  char original_title[ORIGINAL_WINDOW_TITLE_BUFFER];

  /// flag whether it's full screen or not, internally managed.
  bool _fullscreen;

  /// callback available to be set when window's size has changed
  /// This will be called before repainting occurs.
  ///
  /// \param window_id Window id for this callback
  /// \param new_width New window's width
  /// \param new_height New window/s height
  void (*on_window_resize)(Uint32 window_id, int new_width, int new_height);

  /// handle evant callback that can be set.
  /// This function will be called after internal handling event is carried out.
  ///
  /// It's optional to use this.
  void (*handle_event)(SDL_Event* e, float delta_time);

  /// render callback function for this window
  /// Currently it's not used internally just yet, but it will make code cleaner in user's code.
  ///
  /// It's optional to use this.
  void (*render)(float delta_time);
} LWindow;

///
/// Create a new LWindow.
/// Use LWindow_free() to free LWindow later.
/// \param title Title of window
/// \param screen_width Screen width
/// \param screen_height Screen height
/// \param window_flags Additional flags to set for window. It will logical OR with SDL_WINDOW_SHOWN. Use 0 if no additional flags to be set.
/// \param renderer_flags Additional flags to set for renderer. It will logical OR with SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC. Use 0 if no additional flags to be set.
/// \return Newly created LWindow instance on heap.
///
extern LWindow* LWindow_new(const char* title, int screen_width, int screen_height, int window_flags, int renderer_flags);

///
/// Initialize allocated LWindow.
/// Use LWindow_free_internals() to free LWindow created via this function.
/// \param [in,out] window Window instance
/// \param title Title of window
/// \param screen_width Screen width
/// \param screen_height Screen height
/// \param window_flags Flags to set for window. It will logical OR with SDL_WINDOW_SHOWN. Use 0 if no additional flags to be set.
/// \param renderer_flags Additional flags to set for renderer. It will logical OR with SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC. Use 0 if no additional flags to be set.
/// \return True if initialize successfully, otherwise return false.
/// 
extern bool LWindow_init(LWindow *window, const char* title, int screen_width, int screen_height, int window_flags, int renderer_flags);

///
/// Handle event associated with window.
/// \param window Window instance
/// \param e SDL_Event to handle
/// \param delta_time Delta time since last frame
///
extern void LWindow_handle_event(LWindow *window, SDL_Event *e, float delta_time);

///
/// Make window focused
/// \param window Window to make focused
///
extern void LWindow_focus(LWindow* window);

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

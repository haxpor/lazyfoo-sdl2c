#ifndef LBitmapFont_h_
#define LBitmapFont_h_

#include "LTexture.h"

typedef struct {
  /// font texture
  /// (not manage in freeing this attribute)
  LTexture* bitmap;

  /// individual characters in the surface
  SDL_Rect chars[256];

  /// internally used
  int newline;

  /// internally used
  int space;
} LBitmapFont;

///
/// Create a new LBitmapFont
///
/// \return Newly created LBitmapFont allocated memory space on heap.
extern LBitmapFont* LBitmapFont_new();

///
/// Initialize LBitmapFont
///
/// \param bfont LBitmapFont to initialize
/// \return True if initialize successfully, otherwise return false.
///
extern bool LBitmapFont_init(LBitmapFont* bfont);

///
/// Build font
///
/// \param bfont LBitmapFont
/// \param bitmap LTexture to build font from
/// \param num_rows Number of rows for input bitmap
/// \param num_columns Number of column for input bitmap
/// \return True if build font successfully, otherwise return false
///
extern bool LBitmapFont_buildfont(LBitmapFont* bfont, LTexture* bitmap, int num_rows, int num_columns);

///
/// Render text
///
/// \param bfont LBitmapFont
/// \param x Position x to render text at
/// \param y Position y to render text at
/// \param text Text to render
///
extern void LBitmapFont_rendertext(LBitmapFont* bfont, int x, int y, const char* text);

///
/// Measure width and height of text.
///
/// \param bfont LBitmapFont
/// \param text Text to measure its dimensions
/// \param out_text_width Result of text's width
/// \param out_text_height Result of text's height
///
extern void LBitmapFont_measuretext(LBitmapFont* bfont, const char* text, int* out_text_width, int* out_text_height);

///
/// Free internals of LBitmapFonts
///
/// \param bfont LBitmapFont to free its internals
///
extern void LBitmapFont_free_internals(LBitmapFont* bfont);

///
/// Free LBitmapFonts
///
/// \param bfont LBitmapFont to free its allocated memory
///
extern void LBitmapFont_free(LBitmapFont* bfont);
#endif

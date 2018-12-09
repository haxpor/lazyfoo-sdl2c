#include "LBitmapFont.h"
#include <stdlib.h>

static void init_defaults(LBitmapFont* bfont)
{
  bfont->bitmap = NULL;
  bfont->newline = 0;
  bfont->space = 0;
}

LBitmapFont* LBitmapFont_new()
{
  LBitmapFont* out = malloc(sizeof(LBitmapFont));
  
  // init default
  init_defaults(out);

  // init
  if (!LBitmapFont_init(out))
  {
    // free allocated memory immediately
    free(out);
    out = NULL;
  }
  return out;
}

bool LBitmapFont_init(LBitmapFont* bfont)
{
  // nothing to initialize for now
  return true; 
}

bool LBitmapFont_buildfont(LBitmapFont* bfont, LTexture* bitmap, int num_rows, int num_columns)
{
  // go through the bitmap font and define all the clip rectangles for all the sprites.
  
  // lock texture to access its pixel data
  if (!LTexture_LockTexture(bitmap))
  {
    SDL_Log("Cannot lock texture to access its pixel data: %s", SDL_GetError());
    return false;
  }

  // set background color
  // note: position 0,0 needs to be no character bitmap, that area is empty
  // see bitmap file
  Uint32 bgColor = LTexture_GetPixel32(bitmap, 0, 0);

  // set the cell dimensions
  int cell_w = bitmap->width / num_columns;
  int cell_h = bitmap->height / num_rows;

  // new line variable
  int top = cell_h;
  int baseA = cell_h;

  // the current character we're setting
  int current_char = 0;

  // go through the cell rows
  for (int rows = 0; rows < num_rows; rows++)
  {
    // go through the cell columns
    for (int cols = 0; cols < num_columns; cols++)
    {
      // set character offset
      bfont->chars[current_char].x = cell_w * cols;
      bfont->chars[current_char].y = cell_h * rows;

      // set dimensions of character
      bfont->chars[current_char].w = cell_w;
      bfont->chars[current_char].h = cell_h;

      // find left side
      // go through pixel column to find the first non-bg pixel then we mark x position
      for (int pcol=0; pcol<cell_w; pcol++)
      {
        // go through pixel row
        for (int prow=0; prow<cell_h; prow++)
        {
          // find offset to get pixel data
          int x = (cols * cell_w) + pcol;
          int y = (rows * cell_h) + prow;

          if (LTexture_GetPixel32(bitmap, x, y) != bgColor)
          {
            // set x offset
            bfont->chars[current_char].x = x;

            // use goto to break inner and outer loop
            // use 'break' will only break the inner loop
            goto BREAK_THE_LOOP;
          }
        }
      }
BREAK_THE_LOOP:
      // find right side
      for (int pcol = cell_w-1; pcol >= 0; pcol--)
      {
        for (int prow = 0; prow < cell_h; prow++)
        {
          // find offset to get pixel data
          int x = (cols * cell_w) + pcol;
          int y = (rows * cell_h) + prow;

          if (LTexture_GetPixel32(bitmap, x, y) != bgColor)
          {
            // set width
            bfont->chars[current_char].w = (x - bfont->chars[current_char].x) + 1;

            // break the loop
            goto BREAK_THE_LOOP2;
          }
        }
      }
BREAK_THE_LOOP2:
      // find top
      for (int prow=0; prow<cell_h; prow++)
      {
        for (int pcol=0; pcol<cell_w; pcol++)
        {
          // find offset to get pixel data
          int x = (cols * cell_w) + pcol;
          int y = (rows * cell_h) + prow;

          if (LTexture_GetPixel32(bitmap, x, y) != bgColor)
          {
            // if new top if found
            // for height, we want all bitmap chars to have same height
            if (prow < top)
            {
              top = prow;
            }

            // break the loop
            goto BREAK_THE_LOOP3;
          }
        }
      }
BREAK_THE_LOOP3:
      // find bottom
      // only use A as baseline
      if (current_char == 'A')
      {
        for (int prow=cell_h-1; prow>=0; prow--)
        {
          for (int pcol=0; pcol<cell_w; pcol++)
          {
            // find offset to get pixel data
            int x = (cols * cell_w) + pcol;
            int y = (rows * cell_h) + prow;

            if (LTexture_GetPixel32(bitmap, x, y) != bgColor)
            {
              // bottom of A is found
              baseA = prow;

              // break the loop
              goto BREAK_THE_LOOP4;
            }
          }
        }
      }
BREAK_THE_LOOP4:
      // go to next character
      current_char++;
    }
  }
  
  // calculate space
  bfont->space = cell_w / 2;

  // calculate new line
  bfont->newline = baseA - top;

  // lop off excess top pixels
  for (int i=0; i<256; i++)
  {
    bfont->chars[i].y += top;
    bfont->chars[i].h -= top;
  }

  // unlock texture
  LTexture_UnlockTexture(bitmap);
  bfont->bitmap = bitmap;

  return true;
}

void LBitmapFont_rendertext(LBitmapFont* bfont, int x, int y, const char* text)
{
  // if the font has been built
  if (bfont->bitmap == NULL)
  {
    SDL_Log("Bitmap font has not built its font yet. Build first before rendering text.");
    return;
  }

  // temp offset
  int curX = x;
  int curY = y;

  // go through the text
  int str_len = strlen(text);
  for (int i=0; i<str_len; i++)
  {
    // if current character is space
    if (text[i] == ' ')
    {
      // move over
      curX += bfont->space;
    }
    // if current character is a newline
    else if (text[i] == '\n')
    {
      // move down
      curY += bfont->newline;

      // moveback
      curX = x;
    }
    else
    {
      // get ascii value of the character
      int ascii = text[i];

      // show the character
      LTexture_ClippedRender(bfont->bitmap, curX, curY, &bfont->chars[ascii]);

      // move over the width of the character with one pixel of padding
      curX += bfont->chars[ascii].w + 1;
    }
  }
}

void LBitmapFont_measuretext(LBitmapFont* bfont, const char* text, int* out_text_width, int* out_text_height)
{
  if (bfont->bitmap == NULL)
  {
    SDL_Log("Bitmap font has not built its font yet.");
    return;
  } 

  // width/height
  int width = 0;
  int longest_width = 0;
  int height = 0;

  int str_len = strlen(text);
  for (int i=0; i<str_len; i++)
  {
    if (text[i] == ' ')
    {
      width += bfont->space;
    }
    else if (text[i] == '\n')
    {
      height += bfont->newline;
      // save longest width
      if (width > longest_width)
        longest_width = width;
      width = 0;
    }
    else
    {
      int ascii = text[i];
      width += bfont->chars[ascii].w + 1;
    }
  }

  // return result via variables
  if (out_text_width != NULL)
  {
    *out_text_width = width > longest_width ? width : longest_width;
  }
  if (out_text_height != NULL)
  {
    *out_text_height = height;
  }
}

void LBitmapFont_free_internals(LBitmapFont* bfont)
{
  bfont->bitmap = NULL;
  bfont->newline = 0;
  bfont->space = 0;
}

void LBitmapFont_free(LBitmapFont* bfont)
{
  LBitmapFont_free_internals(bfont);

  free(bfont);
  bfont = NULL;
}

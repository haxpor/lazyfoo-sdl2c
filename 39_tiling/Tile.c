#include "Tile.h"
#include <stdlib.h>

Tile* Tile_new(int x, int y, int width, int height, int type)
{
  Tile* out = malloc(sizeof(Tile));

  // init attributes
  if (!Tile_init(out, x, y, width, height, type))
  {
    // free memory immediately
    free(out);
    out = NULL;
  }

  return out;
}

bool Tile_init(Tile* t, int x, int y, int width, int height, int type)
{
  t->type = type;

  t->x = x;
  t->y = y;

  t->box.x = x;
  t->box.y = y;
  t->box.w = width;
  t->box.h = height;

  return true;
}

void Tile_free_internals(Tile* t)
{
  // there's nothing to do just now
}

void Tile_free(Tile* t)
{
  if (t != NULL)
  {
    // free its internals first
    Tile_free_internals(t);

    // free its own allocated memory
    free(t);
    t = NULL;
  }
}

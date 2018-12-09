#ifndef Tile_h_
#define Tile_h_

#include "SDL.h"
#include <stdbool.h>

typedef struct Tile
{
  /// type of tile
  int type;

  // position x
  int x;
  // position y
  int y;

  /// rectangle box covers this tile
  SDL_Rect box;
} Tile;

///
/// Create a new Tile on heap.
///
/// \param x Position x for tile to be placed
/// \param y Position y for tile to be placed
/// \param width Tile width
/// \param height Tile height
/// \param type Type of tile.
/// \return Newly created tile on heap.
///
extern Tile* Tile_new(int x, int y, int width, int height, int type);

///
/// Initialize tile.
///
/// \param t Tile to be initialized
/// \param x Position x for tile to be placed
/// \param y Position y for tile to be placed
/// \param width Tile width
/// \param height Tile height
/// \param type Type of tile.
/// \return True if initialize successfully, otherwise return false.
///
extern bool Tile_init(Tile* t, int x, int y, int width, int height, int type);

///
/// Free internals of Tile.
///
/// \param t Tile to be free its internals.
///
extern void Tile_free_internals(Tile* t);

///
/// Free memory of Tile.
///
/// \param t Tile to be freed.
///
extern void Tile_free(Tile* t);

#endif

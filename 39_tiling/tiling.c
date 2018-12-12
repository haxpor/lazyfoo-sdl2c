/**
 * 39 - Tiling
 */

#include "SDL.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "LWindow.h"
#include "LTexture.h"
#include "LTimer.h"
#include "Tile.h"
#include "Dot.h"
#include "krr_math.h"
#include "bound_sys.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SETFRAME(var, arg1, arg2, arg3, arg4)		\
  do {										\
    var.x = arg1;							\
    var.y = arg2;							\
    var.w = arg3;							\
    var.h = arg4;							\
  } while(0)

// cap thus using fixed deltaTime step
#define TARGET_FPS 60
#define FIXED_DELTATIME 1.0f / TARGET_FPS

// -- functions
bool init();
bool setup();
void update(float deltaTime);
void handleEvent(SDL_Event *e, float deltaTime);
void render(float deltaTime);
void close();

// -- variables
bool quit = false;

// independent time loop
Uint32 currTime = 0;
Uint32 prevTime = 0;

#ifndef DISABLE_FPS_CALC
#define FPS_BUFFER 7+1
char fpsText[FPS_BUFFER];
#endif

#define DOT_SPEED 200

// content's rect to clear color in render loop
SDL_Rect content_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

// as from tiles.png texture
#define TILE_WIDTH 80
#define TILE_HEIGHT 80

// number of tile type
#define TOTAL_TILETYPE 12

// type of tile
enum TileType {
  TILETYPE_RED,
  TILETYPE_GREEN,
  TILETYPE_BLUE,
  TILETYPE_CENTER,
  TILETYPE_TOP,
  TILETYPE_TOPRIGHT,
  TILETYPE_RIGHT,
  TILETYPE_BOTTOMRIGHT,
  TILETYPE_BOTTOM,
  TILETYPE_BOTTOMLEFT,
  TILETYPE_LEFT,
  TILETYPE_TOPLEFT
};
// clipped rects to render tile
SDL_Rect tiles_clipped_rects[TOTAL_TILETYPE] = {
  { 0, 0, TILE_WIDTH, TILE_HEIGHT},
  { 0, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT },
  { 0, TILE_HEIGHT*2, TILE_WIDTH, TILE_HEIGHT },
  { TILE_WIDTH*2, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT },
  { TILE_WIDTH*2, 0, TILE_WIDTH, TILE_HEIGHT },
  { TILE_WIDTH*3, 0, TILE_WIDTH, TILE_HEIGHT },
  { TILE_WIDTH*3, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT },
  { TILE_WIDTH*3, TILE_HEIGHT*2, TILE_WIDTH, TILE_HEIGHT },
  { TILE_WIDTH*2, TILE_HEIGHT*2, TILE_WIDTH, TILE_HEIGHT },
  { TILE_WIDTH, TILE_HEIGHT*2, TILE_WIDTH, TILE_HEIGHT },
  { TILE_WIDTH, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT },
  { TILE_WIDTH, 0, TILE_WIDTH, TILE_HEIGHT }
};

// use dot as player, then camera follows it
Camera cam;
Dot dot;
LTexture* dot_texture = NULL;
BoundSystem bound_system;

// tiles related stuff
// we will read from map file in run-time then allocate memory and initialize attributes
// for all tiles as well as setting then number of tiles (num_tiles)
LTexture* tiles_texture = NULL;
Tile* tiles = NULL;
// map attributes, will be set after reading from map file
int num_tiles = 0;
int map_num_rows;
int map_num_columns;
int level_width;
int level_height;

// check touching of walls (tiles) from circle
bool touch_walls(Circle circle, Tile* tiles, int num_tiles, int* delta_collisionx, int* delta_collisiony)
{
  // go through the tiles
  Tile* tile = NULL;
  for (int i=0; i<num_tiles; i++)
  {
    // get tile
    tile = tiles + i;

    // if the tile is a wall
    if (tile->type >= TILETYPE_CENTER && tile->type <= TILETYPE_TOPLEFT)
    {
      if (krr_math_checkCollision_cr(circle, tile->box, delta_collisionx, delta_collisiony))
      {
        return true;
      }
    }
  }

  return false;
}

#define FILE_BUFFER 1024
// read input mapfile
// return via parameters number of map rows, and number of map columns as seen in parameters
// and out_tiles is an array pointer dynamically created, user has to free it when done using it.
// return true or false based on whether operation is successful.
void read_mapfile(const char* path, Tile** out_tiles, int* out_num_map_rows, int* out_num_map_columns)
{
  FILE* fp = fopen(path, "r");
  if (fp == NULL)
  {
    SDL_Log("error attempting to read  %s file", path);
    return;
  }

  // determine the file size for reading
  fseek(fp, 0, SEEK_END);
  int file_size = ftell(fp);
  // rewind back
  fseek(fp, 0, SEEK_SET);

  // check if file size is more than our buffer can hold
  if (file_size > FILE_BUFFER)
  {
    SDL_Log("file read buffer cannot hold file size more than %d bytes.\nFile input has size %d bytes", FILE_BUFFER, file_size);
    fclose(fp);
    return;
  }

  // buffer to hold read string
  char read_buff[FILE_BUFFER];
  // clear content of read buffer preparing for reading
  memset(read_buff, 0, FILE_BUFFER);

  // read file for FILE_BUFFER bytes in one shot
  int read_objs = fread(read_buff, file_size, 1, fp);
  if (read_objs != 1)
  {
    SDL_Log("error reading file, cannot reading content");
    fclose(fp);
    fp = NULL;
    return;
  }
  // check end of file
  if (feof(fp) > 0)
  {
    const char* error_msg = NULL;
    perror(error_msg);
    SDL_Log("error reading file, end of file : %s", error_msg);
    fclose(fp);
    fp = NULL;
    return;
  }
  // check for error
  if (ferror(fp) > 0)
  {
    const char* error_msg = NULL;
    // get error message
    perror(error_msg);
    SDL_Log("error reading %s file : %s", path, error_msg);
    fclose(fp);
    fp = NULL;
    return;
  }

  // close file
  fclose(fp);
  fp = NULL;

  // get number of lines and number of columns in file from read buffer
  int numlines = 0;
  int numcolumns = 0;
  int str_len = strlen(read_buff);
  bool counting_done = false;

  for (int i=0; i<str_len; i++)
  {
    if (read_buff[i] == '\n')
    {
      numlines++;
      counting_done = true;
    }
    else if (read_buff[i] == ' ' && !counting_done)
    {
      numcolumns++;
    }
  }
  numcolumns++;


  // set to result variables if user inputs it
  if (out_num_map_rows != NULL)
  {
    *out_num_map_rows = numlines;
  }
  if (out_num_map_columns != NULL)
  {
    *out_num_map_columns = numcolumns;
  }

  SDL_Log("num rows: %d", numlines);
  SDL_Log("num cols: %d", numcolumns);

  // dynamically created tile array
  Tile* tiles = malloc(sizeof(Tile) * numlines * numcolumns);
  // delimit both newline and space character
  char delims[] = "\n ";

  char *token_ptr = strtok(read_buff, delims);
  int col_counter = -1;
  int row_counter = 0;
  while (token_ptr != NULL)
  {
    col_counter++;
    if (col_counter >= numcolumns)
    {
      // increment row counter
      row_counter++;
      // reset column counter to new row
      col_counter = 0;
    }

    // calculate byte offset from tiles
    int offset = row_counter * numcolumns + col_counter;
    // convert to type of tile
    int type = atoi(token_ptr);

    // initialize tile individually
    Tile_init(tiles + offset, col_counter * TILE_WIDTH, row_counter * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, type);

    // proceed next
    token_ptr = strtok(NULL, delims);
  }
  
  // set result tiles to output pointer
  // note: now out_tiles points to memory allocated which pointed to by tiles
  // later tiles (pointer) will be free as it's out of scope but
  // our out_tiles will still be there and points to actual allocated memory
  // see https://www.eskimo.com/~scs/cclass/int/sx8.html and https://stackoverflow.com/a/4339219/571227
  // for more info to understand double pointer
  // and modifying value of pointer passed to function
  *out_tiles = tiles;
}

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create window
  gWindow = LWindow_new("39 - Tiling", SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
  if (gWindow == NULL) {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    return false;
  }
  // set device indepenent resolution for rendering (so no unproper aspect ratio)
  if (SDL_RenderSetLogicalSize(gWindow->renderer, SCREEN_WIDTH, SCREEN_HEIGHT) < 0)
  {
    SDL_Log("Warning: failed to set logical size of window");
  }

  // initialize png loading
  // see https://www.libsdl.org/projects/SDL_image/docs/SDL_image.html#SEC8
  // returned from IMG_Init is all flags initted, so we could check for all possible
  // flags we aim for
  int imgFlags = IMG_INIT_PNG;
  int inittedFlags = IMG_Init(imgFlags);
  if ( (inittedFlags & imgFlags) != imgFlags)
  {
    // from document, not always that error string from IMG_GetError() will be set
    // so don't depend on it, just for pure information
    SDL_Log("SDL_Image could not initialize! SDL_image Error: %s", IMG_GetError());
    return false;
  }

#ifndef DISABLE_SDL_TTF_LIB
  // initialize SDL_ttf
  if (TTF_Init() == -1)
  {
    SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
    return false;
  }
#endif

  return true;
}

// include any asset loading sequence, and preparation code here
bool setup()
{
  // load font
  gFont = TTF_OpenFont("../Minecraft.ttf", 16);
  if (gFont == NULL)
  {
    SDL_Log("Failed to load Minecraft.ttf font: %s", TTF_GetError());
    return false;
  }

  // load tiles texture
  tiles_texture = LTexture_LoadFromFile("tiles.png");
  if (tiles_texture == NULL)
  {
    SDL_Log("Failed to create texture from tiles.png");
    return false;
  }

  // read map file (lazy.map) then initialize all tiles instance, and other attributes
  read_mapfile("lazy.map", &tiles, &map_num_rows , &map_num_columns);
  if (tiles == NULL)
  {
    SDL_Log("tiles pointer is null");
  }

  // calculate level width/height
  level_width = TILE_WIDTH * map_num_columns;
  level_height = TILE_HEIGHT * map_num_rows;
  // set total number of tiles
  num_tiles = map_num_rows * map_num_columns;

  // load dot texture
  dot_texture = LTexture_LoadFromFileWithColorKey("dot.bmp", 0xFF, 0xFF, 0xFF);
  if (dot_texture == NULL)
  {
    SDL_Log("Failed to create texture from dot.bmp");
    return false;
  }
  // create dot as player for camera to follow
  Dot_Init(&dot, 100, 100, dot_texture);

  // setup camera
  Camera_init(&cam, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT);
  cam.lerp_factor = 0.20;

  // init bound system once
  BoundSystem_source_init(level_width, level_height);
  // set global function for bounding to our local bound system
  bound_system.f = BoundSystem_global_screenbound_Dot;

  return true;
}

void update(float deltaTime)
{
  Dot_Update(&dot, deltaTime);
  bound_system.f(&dot);
  // check dot against tiles
  int delta_collisionx = 0;
  int delta_collisiony = 0;
  if (touch_walls(dot.collider, tiles, num_tiles, &delta_collisionx, &delta_collisiony))
  {
    // dot touch with walls, then move dot back
    dot.posX -= delta_collisionx;
    dot.posY -= delta_collisiony;
  }

  // update target position of camera to follow dot (after updated position of Dot)
  cam.target_x = (dot.posX + dot.collider.r) - SCREEN_WIDTH/2;
  cam.target_y = (dot.posY + dot.collider.r) - SCREEN_HEIGHT/2;

  Camera_update_lerpcenter(&cam);
  // bound camera
  BoundSystem_bound_Camera(&cam);
}

void handleEvent(SDL_Event *e, float deltaTime)
{
  // user requests quit
  if (e->type == SDL_QUIT ||
      (e->key.keysym.sym == SDLK_ESCAPE))
  {
    quit = true;
  }
  // toggle fullscreen via enter key
  else if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_RETURN)
  {
    if (gWindow->_fullscreen)
    {
      // 0 for windowed mode
      SDL_SetWindowFullscreen(gWindow->window, 0);
      gWindow->_fullscreen = false;
    }
    else
    {
      // SDL_WINDOW_FULLSCREEN_DESKTOP for "fake" fullscreen without changing videomode
      // depends on type of game, and performance aim i.e. FPS game might want to do "real" fullscreen
      // by changing videomode to get performance gain, but point and click with top-down tile-based
      // might not need to change videomode to match the desire spec.
      //
      // as well this needs to work with SDL_RenderSetLogicalSize() function to make it works.
      SDL_SetWindowFullscreen(gWindow->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
      gWindow->_fullscreen = true;
      gWindow->is_minimized = false;
    }
  }

  // dot control
  if (e->type == SDL_KEYDOWN && e->key.repeat == 0)
  {
    switch (e->key.keysym.sym)
    {
      case SDLK_UP:
        dot.targetVelY -= DOT_SPEED * deltaTime;
        break;
      case SDLK_DOWN:
        dot.targetVelY += DOT_SPEED * deltaTime;
        break;
      case SDLK_LEFT:
        dot.targetVelX -= DOT_SPEED * deltaTime;
        break;
      case SDLK_RIGHT:
        dot.targetVelX += DOT_SPEED * deltaTime;
        break;
    }
  }
  else if (e->type == SDL_KEYUP && e->key.repeat == 0)
  {
    switch (e->key.keysym.sym)
    {
      case SDLK_UP:
        dot.targetVelY += DOT_SPEED * deltaTime;
        break;
      case SDLK_DOWN:
        dot.targetVelY -= DOT_SPEED * deltaTime;
        break;
      case SDLK_LEFT:
        dot.targetVelX += DOT_SPEED * deltaTime;
        break;
      case SDLK_RIGHT:
        dot.targetVelX -= DOT_SPEED * deltaTime;
        break;
    }
  }
}

void render(float deltaTime)
{
  if (!gWindow->is_minimized)
  {
    // clear screen (bg)
    SDL_SetRenderDrawColor(gWindow->renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(gWindow->renderer);

    // clear screen (content)
    SDL_SetRenderDrawColor(gWindow->renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderFillRect(gWindow->renderer, &content_rect);

    // check if tile is visible within sight of camera then render it
    int temp_collision_deltax = 0;
    int temp_collision_deltay = 0;

    Tile* temp_tile = NULL;

    for (int i=0; i<num_tiles; i++)
    {
      // get tile
      temp_tile = tiles + i;

      if (krr_math_checkCollision(cam.view_rect, temp_tile->box, &temp_collision_deltax, &temp_collision_deltay))
      {
        LTexture_ClippedRender(tiles_texture, temp_tile->x - cam.view_rect.x, temp_tile->y - cam.view_rect.y, &tiles_clipped_rects[temp_tile->type]);
      }
    }

    Dot_Render_w_camera(&dot, cam.view_rect.x, cam.view_rect.y);

#ifndef DISABLE_FPS_CALC
    // render fps on the top right corner
    snprintf(fpsText, FPS_BUFFER-1, "%d", (int)common_avgFPS);

    // generate fps texture
    SDL_Color color = {30, 30, 30, 255};
    LTexture *fpsTexture = LTexture_LoadFromRenderedText(fpsText, color, 0);
    if (fpsTexture != NULL)
    {
      LTexture_Render(fpsTexture, SCREEN_WIDTH - fpsTexture->width - 5, 10);
      LTexture_Free(fpsTexture);
    }
#endif
  }
}

void close()
{
  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

  // tiles texture
  if (tiles_texture != NULL)
    LTexture_Free(tiles_texture);

  // dot texture
  if (dot_texture != NULL)
    LTexture_Free(dot_texture);

  // tiles
  if (tiles != NULL)
  {
    // free internals for all individual tiles
    for (int i=0; i<num_tiles; i++)
    {
      // free internals, not free it memory space yet
      Tile_free_internals(tiles + i);
    }

    // free allocated memory space
    // note: as allocated space is for all of tiles, free it once for all
    free(tiles);
    tiles = NULL;

    // for safety (but not necessary)
    num_tiles = 0;
  }

  // destroy window
  LWindow_free(gWindow);

#ifndef DISABLE_SDL_TTF_LIB
  TTF_Quit();
#endif

  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char* args[])
{
  // start up SDL and create window
  if (!init())
  {
    SDL_Log("Failed to initialize");
  }	
  else
  {
    // load media, and set up
    if (!setup())
    {
      SDL_Log("Failed to setup!");
    }
    else
    {
      // event handler
      SDL_Event e;

      // while application is running
      while (!quit)
      {
        // prepare delta time to feed to both handleEvent(), update(), and render()
        prevTime = currTime;
        currTime = SDL_GetTicks();
        // calculate per second
        float deltaTime = (currTime - prevTime) / 1000.0f;

#ifndef DISABLE_FPS_CALC
        // fixed step
        common_frameTime += deltaTime;
        common_frameAccumTime += deltaTime;
#endif
        if (common_frameTime >= FIXED_DELTATIME)
        {
#ifndef DISABLE_FPS_CALC
          common_frameCount++;

          // check to reset frame time
          if (common_frameAccumTime >= 1.0f)
          {
            common_avgFPS = common_frameCount / common_frameAccumTime;
            common_frameCount = 0;
            common_frameAccumTime -= 1.0f;
          }
#endif
          common_frameTime = 0.0f;

          // handle events on queue
          // if it's 0, then it has no pending event
          // we keep polling all event in each game loop until there is no more pending one left
          while (SDL_PollEvent(&e) != 0)
          {
            // update user's handleEvent()
            handleEvent(&e, FIXED_DELTATIME);
          }

          update(FIXED_DELTATIME);
          render(FIXED_DELTATIME);
        }
        else {
          render(0); 
        }

        // update screen from any rendering performed since this previous call
        // as we don't use SDL_Surface now, we can't use SDL_UpdateWindowSurface
        SDL_RenderPresent(gWindow->renderer);
      }
    }
  }

  // free resource and close SDL
  close();

  return 0;
}


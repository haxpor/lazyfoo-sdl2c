/**
 * 27 - Collision Detection
 *
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LTexture.h"
#include "LTimer.h"

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

// speed per second
#define DOT_SPEED 200
#define LERP_FACTOR 0.07
typedef struct Dot {
  float posX;         /* position x */
  float posY;         /* position y */
  float velX;         /* velocity x */
  float velY;         /* velocity y */

  float targetVelX;   /* target of velocity x used in lerping */
  float targetVelY;   /* target of velocity y used in lerping */

  LTexture* texture;  /* pointer to texture */

  SDL_Rect collider;  /* collision box */
} Dot;
Dot dot;
LTexture *dotTexture = NULL;
SDL_Rect wall;

/*
 * \brief Lerp from a to b for t.
 * \param a a value
 * \param b b value
 * \param t Values between 0.0-1.0 (inclusion)
 */
float lerp(float a, float b, float t)
{
  return a + (b-a) * t;
}

bool init() {
	// initialize sdl
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
		return false;
	}
	
	// create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
		return false;
	}

	// create renderer for window
	// as we use SDL_Texture, now we need to use renderer to render stuff
	// also use vsync to cap framerate to what video card can do
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL)
	{
		SDL_Log("SDL could not create renderer! SDL_Error: %s", SDL_GetError());
		return false;
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
  gFont = TTF_OpenFont("../8bitwondor.ttf", 16);
  if (gFont == NULL)
  {
    SDL_Log("Failed to load 8bitwonder.ttf font: %s", TTF_GetError());
    return false;
  }

  // load dot texture
  dotTexture = LTexture_LoadFromFile("dot.bmp");
  if (dotTexture == NULL)
  {
    SDL_Log("Failed to load dot.bmp: %s", SDL_GetError());
    return false;
  }
  // set loaded texture to our dot
  dot.texture = dotTexture;
  // set up width and height of collider first (as of information we knew now)
  dot.collider.w = dot.texture->width;
  dot.collider.h = dot.texture->height;

  // wall
  wall.x = 300;
  wall.y = 40;
  wall.w = 40;
  wall.h = 400;

	return true;
}

/*
 * \brief Check collision between collision boxes.
 * \param a Collision box a to check collision
 * \param b Collision box b to check collision
 * \return True if collision occurs, otherwise return false.
 */
bool checkCollision(SDL_Rect a, SDL_Rect b)
{
  if (a.x + a.w > b.x &&
      a.y + a.h > b.y &&
      b.x + b.w > a.x &&
      b.y + b.h > a.y)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void update(float deltaTime)
{
  // lerp on velocity
  dot.velX = lerp(dot.velX, dot.targetVelX, LERP_FACTOR);
  dot.velY = lerp(dot.velY, dot.targetVelY, LERP_FACTOR);
  // calculate the final position
  dot.posX += dot.velX;
  dot.posY += dot.velY;
  dot.collider.x = dot.posX;
  dot.collider.y = dot.posY;

  // bound position
  if (dot.posX < 0 || dot.posX + dot.texture->width > SCREEN_WIDTH || checkCollision(dot.collider, wall))
  {
    // move back
    dot.posX -= dot.velX;
    dot.collider.x = dot.posX;
  }
  if (dot.posY < 0 || dot.posY + dot.texture->height > SCREEN_HEIGHT || checkCollision(dot.collider, wall))
  {
    // move back
    dot.posY -= dot.velY;
    dot.collider.y = dot.posY;
  }
}

void handleEvent(SDL_Event *e, float deltaTime)
{
	// user requests quit
	if (e->type == SDL_QUIT)
	{
		quit = true;
	}
	// user presses a key
	else if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.sym)
		{
		  case SDLK_ESCAPE:
			  quit = true;
        break;
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
  // clear screen
  SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
  SDL_RenderClear(gRenderer);

#ifndef DISABLE_FPS_CALC
  // render fps on the top right corner
  snprintf(fpsText, FPS_BUFFER-1, "%d", (int)common_avgFPS);
  
  // generate fps texture
  SDL_Color color = {30, 30, 30, 255};
  LTexture *fpsTexture = LTexture_LoadFromRenderedText(fpsText, color, 0);
  if (fpsTexture != NULL)
  {
    LTexture_Render(fpsTexture, SCREEN_WIDTH - fpsTexture->width - 5, 0);

    free(fpsTexture);
    fpsTexture = NULL;
  }
#endif

  // render wall
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderDrawRect(gRenderer, &wall);

  // render dot
  LTexture_Render(dot.texture, dot.posX, dot.posY);
}

void close()
{
  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

  // free dot texture
  if (dotTexture != NULL)
  {
    LTexture_Free(dotTexture);
  }

	// destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

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
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	// free resource and close SDL
	close();

	return 0;
}


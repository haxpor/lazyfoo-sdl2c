#include "LTimer.h"
#include <stdlib.h>

LTimer* LTimer_CreateNew()
{
  LTimer *timer = malloc(sizeof(LTimer));

  timer->startedTicks = 0;
  timer->pausedTicks = 0;
  timer->paused = false;
  timer->started = false;

  return timer;
}

void LTimer_Start(LTimer* timer)
{
  timer->started = true;
  timer->paused = false;

  timer->startedTicks = SDL_GetTicks();
  timer->pausedTicks = 0;  
}

void LTimer_Stop(LTimer* timer)
{
  // stop the timer
  timer->started = false;

  // unpause the timer
  timer->paused = false;

  // clear tick variables
  timer->startedTicks = 0;
  timer->pausedTicks = 0;
}

void LTimer_Pause(LTimer* timer)
{
  // if the timer is running and isn't already paused
  if (timer->started && !timer->paused)
  {
    // pause the timer
    timer->paused = true;

    // calculate the paused ticks
    timer->pausedTicks = SDL_GetTicks() - timer->startedTicks;
    timer->startedTicks = 0;
  }
}

void LTimer_Resume(LTimer* timer)
{
  // the the timer is paused and running
  if (timer->started && timer->paused)
  {
    // resume the timer (unpause the timer)
    timer->paused = false;

    // reset the starting ticks
    timer->startedTicks = SDL_GetTicks() - timer->pausedTicks;

    // reset the paused ticks
    timer->pausedTicks = 0;
  }
}

Uint32 LTimer_GetTicks(LTimer* timer)
{
  if (timer->started)
  {
    if (timer->paused)
    {
      return timer->pausedTicks;
    }
    else
    {
      return SDL_GetTicks() - timer->startedTicks;
    }
  }

  // otherwise return 0 as ticks
  return 0;
}

void LTimer_Free(LTimer* timer)
{
  free(timer);
  timer = NULL;
}

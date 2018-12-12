#ifndef LTimer_h_
#define LTimer_h_

#include "SDL.h"
#include <stdbool.h>

typedef struct LTimer {
  // clock time when timer started
  Uint32 startedTicks;

  // clock time when timer paused
  Uint32 pausedTicks;

  // states of timer
  bool paused;
  bool started;
} LTimer;

/*
 * \brief Create a new timer.
 * \return Return a newly created LTimer*
 */
extern LTimer* LTimer_CreateNew();

/*
 * \brief Start the timer.
 * \param timer A timer to start
 */
extern void LTimer_Start(LTimer* timer);

/*
 * \brief Stop the timer.
 * \param timer A timer to stop
 */
extern void LTimer_Stop(LTimer* timer);

/*
 * \brief Pause the timer.
 * \param timer A timer to pause.
 */
extern void LTimer_Pause(LTimer* timer);

/*
 * \brief Resume the paused timer.
 * \param timer A timer to resume.
 */
extern void LTimer_Resume(LTimer* timer);

/*
 * \brief Get current ticks of timer in ms.
 * \param timer A timer to get ticks from
 */
extern Uint32 LTimer_GetTicks(LTimer* timer);

/*
 * \brief Free the timer.
 * \param timer A timer to free from memory.
 *
 * It will set NULL to timer after successfully free it, otherwise it won't.
 */
extern void LTimer_Free(LTimer* timer);

#endif /* LTimer_h_ */

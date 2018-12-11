#ifndef DataStream_h_
#define DataStream_h_

#include "SDL.h"

/// virtually data stream feeder to feed image data to consumer
/// initially used with sample 42 for texture streaming
typedef struct {
  /// current frame (index-based)
  /// (internally used, read-only)
  int current_frame;

  /// counting time for animation
  /// used internally
  float anim_counting_time;
  
  /// there are 4 frames
  /// (read-only)
  SDL_Surface* images[4];

  /// current image
  /// point to one of images dynamically
  /// use this to get SDL_Surface that points to current frame
  /// (read-only)
  SDL_Surface* current_image;
} DataStream;

///
/// Create a new DataStream.
///
/// \return Newly created DataStream.
///
extern DataStream* DataStream_new();

///
/// Update data stream.
///
/// \param d Data stream
/// \param delta_time Delta time since last frame in seconds
///
extern void DataStream_update(DataStream* d, float delta_time);

///
/// Free data stream.
///
extern void DataStream_free(DataStream* d);

#endif

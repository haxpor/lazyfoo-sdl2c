# Changes from original

* Added parameter for related function to accept pixel format especially when create. To make sure it's what user would expect. (Anyway for `DataStream`, we fixed it with `SDL_PIXELFORMAT_RGBA8888` as this struct we don't really care what's behind the scene in reality)

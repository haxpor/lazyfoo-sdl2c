This sample invovles using `SDL2`, `SDL_image`, and `SDL_mixer`.

I proceed this tutorial studying by doing cross compile on Windows from macOS for all libraries and sample source files.

As well, instead of playing only `.wav` file, we play `.mp3` file too.

# Dependencies

## SDL_image

Depends on libpng, and (optional, not use and not link in this sample) XY.

Wheres libpng depends on zlib.

## SDL_mixer

Depends on mpg123 in order to play MP3 file format.

# macOS

Steps in general

1. Compile all required libraries namely `SDL_Image`, and `SDL_mixer`, and all of their required dependencies.
2. Compile sample source file
3. Copy or symlink `libmpg123.dylib` from (usually) `/usr/local/lib/libmpg123.dylib`. The same goes for `libvorbisfile.dylib`.
4. Run the application. (Step 3 is required in run-time but not compile time)

# Windows (cross-compile from macOS)

Steps in generals

1. Build `SDL_image` library
2. Build `SDL_mixer` library
3. Cross compile sample source file
4. Ship it along with required `.dll` files from mingw and depdencies we have

## Build `SDL_image` library

### zlib

* Download source from https://zlib.net/ (slightly scroll down to see link to download)
* Create a new Makefile at `win32/` directory from the root directory of zlib source by using this [patch](https://github.com/madler/zlib/pull/220) as a content for Makefile. You can modify the `HOST` to configure your desire host to build to. Name your Makefile properly.
* It has no configure's options to conveniently set things up, instead execute

```
PREFIX=/tmp/zlib-win32 make -f win32/Makefile.mingw-w32
```

wheres we would like to install zlib at `/tmp/zlib-win32` path, and `Makefile.mingw-w32` is the name of our Makefile newly created from previous step.


### libpng

* Download source from http://www.libpng.org/pub/png/libpng.html (see Source code:)
* Execute

```
./configure --prefix=/tmp/libpng-win32 --with-zlib-prefix=/tmp/zlib-win32 --host=i686-w64-mingw32 CFLAGS="-I/tmp/zlib-win32/include" LDFLAGS="-L/tmp/zlib-win32/lib"
```

assume that you installed `zlib` at `/tmp/zlib-win32`.

### SDL_image

* Download source from https://www.libsdl.org/projects/SDL_image/.
* Go to source directory, then execute the following command

```
./configure --with-sdl-prefix=/tmp/sdl-mingw --prefix=/tmp/SDL_image-win32 --host=i686-w64-mingw32 CFLAGS="-I/usr/local/Cellar/mingw-w64/5.0.4_1/toolchain-i686/i686-w64-mingw32/include -I/tmp/libpng-win32/include" LDFLAGS="-L/usr/local/Cellar/mingw-w64/5.0.4_1/toolchain-i686/i686-w64-mingw32/lib -L/tmp/libpng-win32/lib" --enable-webp=false
```

wheres

+ `--prefix` - specify path to install `SDL_image`
+ `--with-sdl-prefix` - specify copy of SDL (make sure you specify one that is already compiled for target host to run the application, in this case it must be for Windows, not your original SDL2 installed on macOS used for development)
+ `CFLAGS` - specify include path, we specify both for mingw's toolchain and libpng
+ `LDFLAGS` - specify library path for mingw's toolchain and libpng
+ `--enable-webp=false` - I disable webp support, if you don't specify this you have to satisfy its dependencies requirement; thus work further on if you don't specify this.

> Note for all this, I build for i686 which is Windows 32 bit.

## Build `SDL_mixer` library

### mpg123

* Download source from https://www.mpg123.de/download.shtml
* Go to its directory, then execute

```
./configure --prefix=/tmp/mpg123-win32 --host=i686-w64-mingw32 LDFLAG      S=-L/usr/local/Cellar/mingw-w64/5.0.4_1/toolchain-i686/i686-w64-mingw32/lib
```

### SDL_mixer

* Download source from https://www.libsdl.org/projects/SDL_mixer/
* Go to its directory, then execute

```
./configure --with-sdl-prefix=/tmp/sdl-mingw --prefix=/tmp/SDL_mixer-win32 --host=i686-w64-mingw32 LDFLAGS="-L/usr/local/Cellar/mingw-w64/5.0.4_1/toolchain-i686/i686-w64-mingw32/lib -L/tmp/mpg123-win32/lib -L/tmp/mpg123-win32/bin" CFLAGS="-I/tmp/mpg123-win32/include" --enable-music-opus=false
```

in which its option flags are similar to what we've done above with `SDL_image` but it's now to set for `mpg123`. As well, we disable opus via `--enable-music-opus=false` as I can't work out to satisfy its requirement just yet.

## Cross compile sample source file

* Go to our sample source file which is located here (yep that's right you're reading at the correct directory)
* Execute

```
i686-w64-mingw32-gcc sfxmusic.c common.c LTexture.c LButton.c -I/tmp/sdl-mingw/include/SDL2 -L/tmp/sdl-mingw/lib -lmingw32 -lSDL2main -L/tmp/SDL_image-win32/lib -lSDL2_image -L/tmp/SDL_mixer-win32/lib -lSDL2_mixer -lSDL2 -mwindows -I/tmp/SDL_image-win32/include/SDL2 -I/tmp/SDL_mixer-win32/include/SDL2 -DDISABLE_SDL_TTF_LIB -I. -o sfxmusic.exe
```

Note that we specify `-DDISABLE_SDL_TTF_LIB` to disable compilation and linking of `SDL_ttf` as we don't need it in this sample. Others we just need to specify correct path of library to our cross-compiled libraries as we've done above. Include all `.c` source files to compile, then finally spit out `.exe` file via `-o ...`.

## Ship it

* Get all required assets used in run-time namedly all `.wav`, `.mp3` and `.png` files.
* Get all required `.dll` which are `SDL2.dll`, `SDL2_image.dll`, `SDL2_mixer.dll`, `libgcc_s_sjlj-1.dll`, `libmmpg123-0.dll`, `libpng16-16.dll`, `libwinpthread-1.dll`, and `zlib1.dll`. Note that you can get releveant `.dll` file from its `<installation-path>/bin`. Other than that, look at your mingw's toolchain's `bin` and `lib` directory.
* Copy `.exe` file along with files from previous two steps to your Windows 32 bit machine.
* Run and done!

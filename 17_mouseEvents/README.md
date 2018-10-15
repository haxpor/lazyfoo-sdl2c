# mouseevents

`Makefile` has a special handling when building this sample.

As `LTexture` grows bigger in functionality, it links with more SDL2's extension namely `SDL_image` and `SDL_ttf`. It could be more lightweight if we could produce less in filesize of final executable file.

So this sample takes this chance to do this following original tutorial.

In original tutorial, Lazyfoo uses `#ifdef _SDL_TTF_H` which used by `SDL_ttf` to check if such header file is included then certain relevant code section for TTF would be there and involved in compilation process.

This sample use `-DDISABLE_SDL_TTF_LIB` at `Makefile` level to indicate that it doesn't need `SDL_ttf` to compile. Notice that it doesn't use `#define DISABLE_SDL_TTF_LIB` in source file as it needs to be in the same compilation unit of `LTexture`; which is totally not convenient for user to do so, and it requires source modification.

Another one is handler function for `LButton` which implemented as function pointer inside struct in C.

Thus check out `Makefile` for more information.

# Changes from previous samples

* Update `LTexture`'s `LTexture_LoadFromFile` and `LTexture_LoadFromRenderedText` to return `LTexture*` instead of boolean, and without accepting any pointer to return output created object. It will always create a new struct on heap via `malloc()`.
* Update `LTexture`'s `LTexture_Free()` to free and set to NULL for input pointer. The same way and aligned with what its creation functions did.
* Changed flag `NLIB_SDL_TTF` to `DISABLE_SDL_TTF_LIB`.

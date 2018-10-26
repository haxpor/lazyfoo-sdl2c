# this Makefile although LTexture supports TTF now, but
# it doesn't need in this sample, thus -DDISABLE_SDL_TTF_LIB
#
# If build the project dynamically, we need to put both
# dynamica library files of libmpg123.dylib and libvorbisfile.dylib
# (in case of macOS) at the same directory level of executable file.
# If not so, at the code checking return result of Mix_Int() against
# bitmasked flags, it will report such format is not supported.
PROGRAM=adv_timing
OUTPUT=adv_timing

CC = gcc
EXE = .out
CFLAGS = -std=c99 -Wall -I. -I/usr/local/include/SDL2
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
TARGETS = \
	  common.o \
	  LTexture.o \
	  LTimer.o \
	  $(PROGRAM).o \
	  $(OUTPUT)

.PHONY: all clean $(PROGRAM)

all: $(TARGETS)

static: $(STATIC_TARGETS)

$(OUTPUT): $(PROGRAM).o LTexture.o common.o
	$(CC) $(PROGRAM).o LTexture.o common.o LTimer.o -o $(OUTPUT)$(EXE) $(LIBS)

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c -o common.o

LTexture.o: LTexture.c LTexture.h
	$(CC) $(CFLAGS) -c LTexture.c -o LTexture.o

LTimer.o: LTimer.c LTimer.h
	$(CC) $(CFLAGS) -c LTimer.c -o LTimer.o

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) -c $(PROGRAM).c -o $(PROGRAM).o

clean:
	rm -rf *.out *.o

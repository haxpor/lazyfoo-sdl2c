# User can put custom cflags via USER_CFLAGS
PROGRAM=framerate
OUTPUT=framerate

CC = gcc
EXE = .out
CFLAGS = -std=c99 -Wall -I. -I/usr/local/include/SDL2 -g
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
USER_CFLAGS ?= -DCALCULATE_WITH_DELTATIME=1
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
	$(CC) $(CFLAGS) $(USER_CFLAGS) -c $(PROGRAM).c -o $(PROGRAM).o

clean:
	rm -rf *.out *.o

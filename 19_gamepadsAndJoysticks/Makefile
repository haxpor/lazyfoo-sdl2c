# this Makefile although LTexture supports TTF now, but
# it doesn't need in this sample, thus -DDISABLE_SDL_TTF_LIB
PROGRAM=joysticks
OUTPUT=joysticks

PROGRAM2=controllers
OUTPUT2=controllers

CC = gcc
EXE = .out
CFLAGS = -std=c99 -Wall -I. -DDISABLE_SDL_TTF_LIB
LIBS = -lsdl2 -lsdl2_image
JOYSTICK_TARGETS = \
	  common.o \
	  LTexture.o \
	  $(PROGRAM).o \
	  $(OUTPUT)
CONTROLLER_TARGETS = \
		     common.o \
		     LTexture.o \
		     $(PROGRAM2).o \
		     $(OUTPUT2)

.PHONY: all clean joysticks controllers

all: $(JOYSTICK_TARGETS) $(CONTROLLER_TARGETS)

$(PROGRAM)-sample: $(JOYSTICK_TARGETS)

$(PROGRAM2)-sample: $(CONTROLLER_TARGETS)

$(OUTPUT): $(PROGRAM).o LTexture.o common.o
	$(CC) $(PROGRAM).o LTexture.o common.o -o $(OUTPUT)$(EXE) $(LIBS)

$(OUTPUT2): $(PROGRAM2).o LTexture.o common.o
	$(CC) $(PROGRAM2).o LTexture.o common.o -o $(OUTPUT2)$(EXE) $(LIBS)

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c -o common.o

LTexture.o: LTexture.c LTexture.h
	$(CC) $(CFLAGS) -c LTexture.c -o LTexture.o

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) -c $(PROGRAM).c -o $(PROGRAM).o

$(PROGRAM2).o: $(PROGRAM2).c
	$(CC) $(CFLAGS) -c $(PROGRAM2).c -o $(PROGRAM2).o

clean:
	rm -rf *.out *.o

# this Makefile although LTexture supports TTF now, but
# it doesn't need in this sample, thus -DDISABLE_SDL_TTF_LIB
PROGRAM=keystates
OUTPUT=keystates

CC = gcc
EXE = .out
CFLAGS = -std=c99 -Wall -I. -DDISABLE_SDL_TTF_LIB
LIBS = -lsdl2 -lsdl2_image
TARGETS = \
	  common.o \
	  LTexture.o \
	  $(PROGRAM).o \
	  $(OUTPUT)

all: $(TARGETS)

$(OUTPUT): $(PROGRAM).o LTexture.o common.o
	$(CC) $(PROGRAM).o LTexture.o common.o -o $(OUTPUT)$(EXE) $(LIBS)

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c -o common.o

LTexture.o: LTexture.c LTexture.h
	$(CC) $(CFLAGS) -c LTexture.c -o LTexture.o

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) -c $(PROGRAM).c -o $(PROGRAM).o

clean:
	rm -rf $(OUTPUT) *.o

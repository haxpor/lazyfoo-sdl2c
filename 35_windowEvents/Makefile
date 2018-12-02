PROGRAM=windowevents
OUTPUT=windowevents

CC = gcc
EXE = .out
override CFLAGS += -std=c99 -Wall -I. -I/usr/local/include/SDL2
override LIBS += -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
TARGETS = \
	  common.o \
	  krr_math.o \
	  LWindow.o \
	  LTexture.o \
	  LTimer.o \
	  $(PROGRAM).o \
	  $(OUTPUT)

.PHONY: all clean

all: $(TARGETS) 

$(OUTPUT): $(PROGRAM).o LWindow.o LTexture.o common.o krr_math.o
	$(CC) $^ -o $(OUTPUT)$(EXE) $(LIBS)

common.o: common.c common.h
	$(CC) $(CFLAGS) -c $<  -o $@

krr_math.o: krr_math.c krr_math.h
	$(CC) $(CFLAGS) -c $< -o $@

LWindow.o: LWindow.c LWindow.h
	$(CC) $(CFLAGS) -c $< -o $@

LTexture.o: LTexture.c LTexture.h
	$(CC) $(CFLAGS) -c $< -o $@

LTimer.o: LTimer.c LTimer.h
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) -c $(PROGRAM).c -o $(PROGRAM).o

clean:
	rm -rf *.out *.o *.dSYM

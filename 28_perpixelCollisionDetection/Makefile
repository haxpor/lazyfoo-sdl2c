PROGRAM=perpixelc
OUTPUT=perpixelc

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

TARGETS_VECTORDOT_TEST = \
	  vectorDot.o \
	  vectorDot_test.o \
	  vectorDot

TARGETS_VECTOR_TEST = \
	  vector.o \
	  vector_test.o \
	  vector

.PHONY: all clean vectorDot vector

all: $(TARGETS) $(TARGETS_VECTORDOT_TEST) $(TARGETS_VECTOR_TEST)

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

vectorDot.o: vectorDot.c Dot.h common_debug.h
	$(CC) $(CFLAGS) -c $< -o $@

vectorDot_test.o: vectorDot_test.c vectorDot.h Dot.h
	$(CC) $(CFLAGS) -c $< -o $@

vectorDot: vectorDot.o vectorDot_test.o
	$(CC) $^ -o $@  $(LIBS)

vector.o: vector.c common_debug.h
	$(CC) $(CFLAGS) -c $< -o $@

vector_test.o: vector_test.c
	$(CC) $(CFLAGS) -c $< -o $@

vector: vector.o vector_test.o
	$(CC) $^ -o $@

clean:
	rm -rf *.out *.o *.dSYM

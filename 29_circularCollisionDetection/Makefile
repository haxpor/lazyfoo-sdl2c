PROGRAM=circularc
OUTPUT=circularc

CC = gcc
EXE = .out
override CFLAGS += -std=c99 -Wall -I. -I/usr/local/include/SDL2
override LIBS += -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
TARGETS = \
	  common.o \
	  krr_math.o \
	  LTexture.o \
	  LTimer.o \
	  Dot.o \
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

.PHONY: all clean vectorDot vector Dot krr_math

all: $(TARGETS) 

vector_test: $(TARGETS_VECTORDOT_TEST) $(TARGETS_VECTOR_TEST)

$(OUTPUT): $(PROGRAM).o LTexture.o common.o Dot.o krr_math.o LTimer.o
	$(CC) $^ -o $(OUTPUT)$(EXE) $(LIBS)

common.o: common.c common.h
	$(CC) $(CFLAGS) -c $<  -o $@

krr_math.o: krr_math.c krr_math.h
	$(CC) $(CFLAGS) -c $< -o $@

LTexture.o: LTexture.c LTexture.h
	$(CC) $(CFLAGS) -c $< -o $@

LTimer.o: LTimer.c LTimer.h
	$(CC) $(CFLAGS) -c $< -o $@

Dot.o: Dot.c Dot.h common.o
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM).o: $(PROGRAM).c krr_math.c LTexture.c LTimer.c Dot.c
	$(CC) $(CFLAGS) -c $(PROGRAM).c -o $(PROGRAM).o

vectorDot.o: vectorDot.c Dot.h common_debug.h
	$(CC) $(CFLAGS) -c $< -o $@

vectorDot_test.o: vectorDot_test.c vectorDot.h Dot.h
	$(CC) $(CFLAGS) -c $< -o $@

vectorDot: vectorDot.o vectorDot_test.o
	$(CC) $^ -o $@$(EXE)  $(LIBS)

vector.o: vector.c common_debug.h
	$(CC) $(CFLAGS) -c $< -o $@

vector_test.o: vector_test.c
	$(CC) $(CFLAGS) -c $< -o $@

vector: vector.o vector_test.o
	$(CC) $^ -o $@$(EXE)

clean:
	rm -rf *.out *.o *.dSYM

PROGRAM=scrolling
OUTPUT=scrolling

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
	  bound_sys.o \
	  camera.o \
	  $(PROGRAM).o \
	  $(OUTPUT)

.PHONY: all clean

all: $(TARGETS) 

$(OUTPUT): $(PROGRAM).o LTexture.o common.o Dot.o krr_math.o LTimer.o bound_sys.o camera.o
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

bound_sys.o: bound_sys.c bound_sys.h
	$(CC) $(CFLAGS) -c $< -o $@

camera.o: camera.c camera.h
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM).o: $(PROGRAM).c krr_math.c LTexture.c LTimer.c Dot.c
	$(CC) $(CFLAGS) -c $(PROGRAM).c -o $(PROGRAM).o

clean:
	rm -rf *.out *.o *.dSYM

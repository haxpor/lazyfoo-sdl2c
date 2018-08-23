PROGRAM=colormod
OUTPUT=colormod

all: $(OUTPUT)

$(OUTPUT): $(PROGRAM).o LTexture.o common.o
	gcc $(PROGRAM).o LTexture.o common.o -o $(OUTPUT) -lsdl2 -lsdl2_image

common.o: common.c common.h
	gcc -std=c99 -Wall -c common.c -o common.o -I.

LTexture.o: LTexture.c LTexture.h
	gcc -std=c99 -Wall -c LTexture.c -o LTexture.o -I.

$(PROGRAM).o: $(PROGRAM).c
	gcc -std=c99 -Wall -c $(PROGRAM).c -o $(PROGRAM).o -I.

clean:
	rm -rf $(OUTPUT)
	rm -rf *.o

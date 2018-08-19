PROGRAM=extlibimgf
OUTPUT=extlibimgf

$(OUTPUT): $(PROGRAM).c
	gcc -std=c99 -Wall -o $(OUTPUT) $(PROGRAM).c -lsdl2 -lsdl2_image
debug: $(PROGRAM).c
	gcc -std=c99 -Wall -g -o $(OUTPUT) $(PROGRAM).c -lsdl2 -lsdl2_image
clean:
	rm -rf $(OUTPUT)

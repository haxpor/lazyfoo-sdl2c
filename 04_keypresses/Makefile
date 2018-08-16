PROGRAM=keypresses
OUTPUT=keypresses

$(OUTPUT): $(PROGRAM).c
	gcc -std=c99 -Wall -o $(OUTPUT) $(PROGRAM).c -lsdl2
debug: $(PROGRAM).c
	gcc -std=c99 -Wall -g -o $(OUTPUT) $(PROGRAM).c -lsdl2
clean:
	rm -rf $(OUTPUT)

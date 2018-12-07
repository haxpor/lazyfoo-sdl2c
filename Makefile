# get all directories that we're going to build
DIRS = $(shell find . -maxdepth 1 -type d -not -path ./.git -not -path .)

.PHONY: all clean

all:
	for dir in $(DIRS); do \
		echo $$dir ; \
		cd $$dir && make && cd .. ; \
	done

clean:
	for dir in $(DIRS); do \
		echo $$dir ; \
		cd $$dir && make clean && cd .. ; \
	done

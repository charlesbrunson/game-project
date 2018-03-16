# A simple makefile for linux version

CXX=g++
CFLAG=-std=c++14 -g -Wall -Wextra -D_DEBUG -I SFML/include -I incl -I incl/ext
LIB=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

CPP=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp,obj/%.o,$(CPP))

obj/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAG) -c $< -o $@

sfml-game: $(OBJ)
	#cp -r -u src/data/ .
	$(CXX) -pthread $^ -o $@ $(LIB)
	echo "#!/bin/sh\n./sfml-game" > run.sh
	chmod +x run.sh

.PHONY : clean
clean :
	#rm -f -r data/
	rm -f sfml-game
	rm -f -r obj/
